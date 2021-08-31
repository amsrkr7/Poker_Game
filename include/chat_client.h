#ifndef CHAT_CLIENT_H
#define CHAT_CLIENT_H

#include <cstdlib>
#include <deque>
#include <iostream>
#include <thread>
#include "asio.hpp"
#include "chat_message.h"
#include "player_interface.h"
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <string>
#include <chrono>
#include <ctime>
#include <mutex>
class Gui;

typedef std::deque<chat_message> chat_message_queue;

using asio::ip::tcp;

class chat_client
{
public:
    chat_client(asio::io_context& io_context,
                const tcp::resolver::results_type& endpoints, Player_Interface* p);

    // CSE3310 write() sends the message to the server
    void write(const chat_message& msg);
    void close();
    void link_gui(Gui * g);
    bool has_gui;

private:
    // CSE3310 do_connect establishes connection with the server
    void do_connect(const tcp::resolver::results_type& endpoints);
    void do_read_header();
    void do_read_body();
    void do_write();

private:
    boost::uuids::random_generator gen;
    asio::io_context& io_context_;
    tcp::socket socket_;
    chat_message read_msg_;
    chat_message_queue write_msgs_;
    Player_Interface* player;
    std::mutex myLock;
    Gui * gui;
};

#endif
