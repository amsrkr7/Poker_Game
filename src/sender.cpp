#include <cstdlib>
#include <iostream>

#include "chat_client.h"
#include "sender.h"
#include "player_interface.h"
#include <boost/uuid/uuid_io.hpp>
#include <json.hpp>

using json = nlohmann::json;

Sender::Sender(Player_Interface *p) : gamer{p}
{
}

//send raise message
void Sender::bet_raise(chat_client *c, int amount)
{
    chat_message msg;
    json j1;

    j1["bet amount"] = amount;
    j1["chat"] = "Please raise the bet.";
    j1["event"] = "bet_raise";
    j1 ["from"] = {{"name",gamer->getName()},{"uuid", boost::uuids::to_string(gamer->getUUID())}};

    std:: string s = j1.dump();

    msg.body_length(s.size());
    std::memcpy(msg.body(), s.c_str(), msg.body_length());
    msg.encode_header();
    c->write(msg);
}

//sends call message
void Sender::call_check(chat_client *c)
{
    chat_message msg;
    json j1;


    j1 ["chat"] = "I want to call.";
    j1["event"] = "checked";
    j1 ["from"] = {{"name",gamer->getName()},{"uuid", boost::uuids::to_string(gamer->getUUID())}};

    std:: string s = j1.dump();


    msg.body_length(s.size());
    std::memcpy(msg.body(), s.c_str(), msg.body_length());
    msg.encode_header();
    c->write(msg);

}

//sends fold message
void Sender::fold(chat_client *c)
{
    chat_message msg;
    json j1;


    j1 ["chat"] = "I want to fold.";
    j1["event"]= "folded";
    j1 ["from"] = {{"name",gamer->getName()},{"uuid", boost::uuids::to_string(gamer->getUUID())}};


    std:: string s = j1.dump();

    msg.body_length(s.size());
    std::memcpy(msg.body(), s.c_str(), msg.body_length());
    msg.encode_header();
    c->write(msg);

}

//sends swap message
void Sender::swap(chat_client *c, bool (&cards)[5])
{
    chat_message msg;
//    int i;
//    int count = 0;
    json j1;

    j1["chat"] = "Player wants to swap the card";
    /*
        for(i =0; i < 5; i++)
        {
        	if (cards[i] == 1)
        	{
        		count ++;
        	}
        }
    */
    j1["event"]= "swap";
    j1["to_swap"] = cards;
    j1 ["from"] = {{"name",gamer->getName()},{"uuid", boost::uuids::to_string(gamer->getUUID())}};

    std:: string s = j1.dump();

    msg.body_length(s.size());
    std::memcpy(msg.body(), s.c_str(), msg.body_length());
    msg.encode_header();
    c->write(msg);
}

void Sender::ready(chat_client *c)
{
    chat_message msg;

    json j1;

    j1["event"] = "player_join";
    j1["from"] = {{"name",gamer->getName()},{"uuid", boost::uuids::to_string(gamer->getUUID())}};

    std::string s = j1.dump();

    msg.body_length(s.size());
    std::memcpy(msg.body(), s.c_str(), msg.body_length());
    msg.encode_header();
    c->write(msg);
}