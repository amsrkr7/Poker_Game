// chat_client.cpp
// ~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2018 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <cstdlib>
#include <deque>
#include <iostream>
#include <thread>
#include "asio.hpp"
#include "chat_message.h"

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/lexical_cast.hpp>
#include <string>
#include <chrono>
#include <ctime>

#include <chat_client.h>
#include <gui.h>

#ifndef MAX_SIZE
#define MAX_SIZE 4096
#endif

using asio::ip::tcp;


chat_client::chat_client(asio::io_context& io_context,
                         const tcp::resolver::results_type& endpoints, Player_Interface* p)
    : io_context_(io_context),
      socket_(io_context),
      player{p}
{

    this->has_gui = false;

    boost::uuids::uuid newID = gen();
    player->setUUID(newID);

    std::string s = "uuid:";
    s+= boost::uuids::to_string(newID);
    std::cout << "\ncreating " << s << std::endl;

    chat_message msg;
    msg.body_length(s.size());
    std::memcpy(msg.body(), s.c_str(), msg.body_length());
    msg.encode_header();
    this->write(msg);

    do_connect(endpoints);
}

// CSE3310 write() sends the message to the server
void chat_client::write(const chat_message& msg)
{
    asio::post(io_context_,
               [this, msg]()
    {
        bool write_in_progress = !write_msgs_.empty();
        write_msgs_.push_back(msg);
        if (!write_in_progress)
        {
            do_write();

        }
    });
}

void chat_client::close()
{
    asio::post(io_context_, [this]() {
        socket_.close();
    });
}

// CSE3310 do_connect establishes connection with the server
void chat_client::do_connect(const tcp::resolver::results_type& endpoints)
{
    asio::async_connect(socket_, endpoints,
                        [this](std::error_code ec, tcp::endpoint)
    {
        if (!ec)
        {
            do_read_header();
        }
    });
}

void chat_client::do_read_header()
{
    asio::async_read(socket_,
                     asio::buffer(read_msg_.data(), chat_message::header_length),
                     [this](std::error_code ec, std::size_t /*length*/)
    {
        if (!ec && read_msg_.decode_header())
        {
            do_read_body();
        }
        else
        {
            socket_.close();
        }
    });
}

void chat_client::do_read_body()
{
    asio::async_read(socket_,
                     asio::buffer(read_msg_.body(), read_msg_.body_length()),
                     [this](std::error_code ec, std::size_t /*length*/)
    {
        if (!ec)
        {
            // CSE3310 message body is received and printed
            //std::cout.write(read_msg_.body(), read_msg_.body_length());
            //std::cout << "\n\n";

            // THIS IS NOT THE RIGHT WAY TO DO THIS, ITS A HACK
            while(!has_gui) {} // the thread will be stuck in this loop until it has a gui
            {
                gdk_threads_enter();
                gui->update(read_msg_);
                gdk_threads_leave();
            }
            do_read_header();
        }
        else
        {
            socket_.close();
        }
    });
}

void chat_client::do_write()
{
    asio::async_write(socket_,
                      asio::buffer(write_msgs_.front().data(),
                                   write_msgs_.front().length()),
                      [this](std::error_code ec, std::size_t /*length*/)
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
            socket_.close();
        }
    });
}

// This function links the gui pointer to the chat_client object, and sets the
// gui_set bool to true
void chat_client::link_gui(Gui * g) {

    this->gui = g;
    this->has_gui = true;

}
