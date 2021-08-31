//
// chat_server.cpp
// ~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2018 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef CHAT_SERVER_H
#define CHAT_SERVER_H

#include <cstdlib>
#include <deque>
#include <iostream>
#include <list>
#include <memory>
#include <set>
#include <utility>
#include <queue>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include "asio.hpp"
#include "chat_message.h"
#include "broadcast.h"
#include "player.h"

using asio::ip::tcp;
class Broadcast;
//----------------------------------------------------------------------

typedef std::deque<chat_message> chat_message_queue;

//----------------------------------------------------------------------

class chat_participant
{
public:
    virtual ~chat_participant() {}
    virtual void deliver(const chat_message& msg) = 0;
};

typedef std::shared_ptr<chat_participant> chat_participant_ptr;

//----------------------------------------------------------------------

class chat_room
{
public:
    void join(chat_participant_ptr participant);

    void leave(chat_participant_ptr participant);

    void deliver(const chat_message& msg);

private:
    std::set<chat_participant_ptr> participants_;
    enum { max_recent_msgs = 100 };
    chat_message_queue recent_msgs_;
};

//----------------------------------------------------------------------

class chat_session
    : public chat_participant,
      public std::enable_shared_from_this<chat_session>
{
public:
    chat_session(tcp::socket socket, chat_room& room, std::queue<chat_message>* Q, std::vector<Player*>* p);

    void start();

    void deliver(const chat_message& msg);

private:
    void do_read_header();

    void do_read_body();

    void do_write();

    tcp::socket socket_;
    chat_room& room_;
    chat_message read_msg_;
    chat_message_queue write_msgs_;
    std::queue<chat_message>* message_queue;
    std::vector<Player*>* players_;
};

//----------------------------------------------------------------------

class chat_server
{
public:
    chat_server(asio::io_context& io_context,
                const tcp::endpoint& endpoint, std::queue<chat_message>* Q, Broadcast* b, std::vector<Player*>* p);
private:
    void do_accept();

    tcp::acceptor acceptor_;
    chat_room room_;
    std::queue<chat_message>* message_queue;
    Broadcast* bcast;
    std::vector<Player*>* players_;
};


#endif
