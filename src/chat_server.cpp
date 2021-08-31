#include <cstdlib>
#include <deque>
#include <queue>
#include <iostream>
#include <list>
#include <memory>
#include <set>
#include <utility>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/lexical_cast.hpp>
#include "asio.hpp"
#include "chat_message.h"
#include "broadcast.h"
#include "player.h"

#ifndef MAX_SIZE
#define MAX_SIZE 4096
#endif


using asio::ip::tcp;



void chat_room::join(chat_participant_ptr participant)
{
    participants_.insert(participant);

    // CSE3310 this for loop sends all the previous messages to the new participant
    for (auto msg: recent_msgs_)
        participant->deliver(msg);
}

void chat_room::leave(chat_participant_ptr participant)
{
    participants_.erase(participant);
}

void chat_room::deliver(const chat_message& msg)
{
    if(std::strncmp(msg.body(), "uuid:", 5) != 0)
    {
        recent_msgs_.push_back(msg);
        while (recent_msgs_.size() > max_recent_msgs)
            recent_msgs_.pop_front();
    }
    // CSE3310 this for loop delivers the message to all participants
    for (auto participant: participants_)
        participant->deliver(msg);
}

//----------------------------------------------------------------------------
chat_session::chat_session(tcp::socket socket, chat_room& room, std::queue<chat_message>* Q, std::vector<Player*>* p)
    : socket_(std::move(socket)),
      room_(room),
      message_queue{Q},
      players_{p}
{
}

void chat_session::start()
{

    room_.join(shared_from_this());
    do_read_header();
}

void chat_session::deliver(const chat_message& msg)
{
    bool write_in_progress = !write_msgs_.empty();
    write_msgs_.push_back(msg);
    if (!write_in_progress)
    {
        do_write();
    }
}

void chat_session::do_read_header()
{
    auto self(shared_from_this());
    asio::async_read(socket_,
                     asio::buffer(read_msg_.data(), chat_message::header_length),
                     [this, self](std::error_code ec, std::size_t /*length*/)
    {
        if (!ec && read_msg_.decode_header())
        {
            do_read_body();
        }
        else
        {
            room_.leave(shared_from_this());
        }
    });
}

void chat_session::do_read_body()
{
    auto self(shared_from_this());
    asio::async_read(socket_,
                     asio::buffer(read_msg_.body(), read_msg_.body_length()),
                     [this, self](std::error_code ec, std::size_t /*length*/)
    {
        if (!ec)
        {

            char content_[MAX_SIZE];

            //copies the content of the message to a string
            std::strncpy(content_, read_msg_.body(), read_msg_.body_length());
            content_[read_msg_.body_length()] = '\0';

            if(std::strncmp(content_, "uuid:", 5) == 0)
            {
                char* c_pos = content_;
                boost::uuids::uuid newID = boost::lexical_cast<boost::uuids::uuid>(c_pos + 5);
                players_->push_back(new Player(newID));
                std::cout << "new ID : " << newID << std::endl;
            }
            else
            {
                room_.deliver(read_msg_);
                message_queue->push(read_msg_);
            }
            do_read_header();
        }
        else
        {
            room_.leave(shared_from_this());
        }
    });
}

void chat_session::do_write()
{
    auto self(shared_from_this());
    asio::async_write(socket_,
                      asio::buffer(write_msgs_.front().data(),
                                   write_msgs_.front().length()),
                      [this, self](std::error_code ec, std::size_t /*length*/)
    {
        if (!ec)
        {
            write_msgs_.pop_front();
            if (!write_msgs_.empty())
            {
                do_write();
            }
        }
        else
        {
            room_.leave(shared_from_this());
        }
    });
}
//-----------------------------------------------------------------
chat_server::chat_server(asio::io_context& io_context,
                         const tcp::endpoint& endpoint, std::queue<chat_message>* Q, Broadcast* b, std::vector<Player*> *p)
    : acceptor_(io_context, endpoint), message_queue{Q}, bcast{b}, players_{p}
{
    bcast->link_room(&room_);
    do_accept();
}

void chat_server::do_accept()
{

    acceptor_.async_accept(
        [this](std::error_code ec, tcp::socket socket)
    {
        if (!ec)
        {
            std::make_shared<chat_session>(std::move(socket), room_, message_queue, players_)->start();
        }
        do_accept();
    });
}

