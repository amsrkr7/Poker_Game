#include "broadcast.h"
#include "chat_server.h"
#include <string>
#include <iostream>
//links a room to send to the clients
void Broadcast::link_room(chat_room* cs)
{
    room = cs;
}

//makes sure a room is linked before continuing
bool Broadcast::verify()
{
    return room!=NULL;
}

//sends ante message
void Broadcast::ante_message(json game_info, int ante_amount)
{
    game_info["possible_actions"] = {"no","call", "fold", "no"};
    game_info["bet_amount"] = ante_amount;
    std::string s = game_info.dump();
    chat_message msg;

    msg.body_length(s.size());
    std::memcpy(msg.body(), s.c_str(), msg.body_length());
    msg.encode_header();
    room->deliver(msg);
}

//sends the cards to everyone
void Broadcast::distribute_message(json game_info)
{
    std::string s = game_info.dump();
    chat_message msg;

    msg.body_length(s.size());
    std::memcpy(msg.body(), s.c_str(), msg.body_length());
    msg.encode_header();
    room->deliver(msg);
}

//send bet message
void Broadcast::betting_message(json game_info, int bet_amount, int min, std::string uuid)
{

    game_info["possible_actions"] = {"bet", "call", "fold", "no"};
    game_info["bet_amount"] = bet_amount;
    game_info["min bet"] = min;
    game_info["turn"] = uuid;
    std:: string s = game_info.dump();
    chat_message msg;

    msg.body_length(s.size());
    std::memcpy(msg.body(), s.c_str(), msg.body_length());
    msg.encode_header();
    room->deliver(msg);
}

//send swap message
void Broadcast::swap_message(json game_info)
{
    game_info["possible_actions"] = {"no","no","no","swap"};

    std:: string s = game_info.dump();
    chat_message msg;
    msg.body_length(s.size());
    std::memcpy(msg.body(), s.c_str(), msg.body_length());
    msg.encode_header();
    room->deliver(msg);
}

void Broadcast::win_message(json game_info)
{
    game_info["possible_actions"] = {"bet", "call", "fold", "swap"};
    std:: string s = game_info.dump();
    chat_message msg;
    msg.body_length(s.size());
    std::memcpy(msg.body(), s.c_str(), msg.body_length());
    msg.encode_header();
    room->deliver(msg);
}
