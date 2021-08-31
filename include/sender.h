#ifndef POKER_H
#define POKER_H

#include "chat_client.h"
#include "player_interface.h"
#include <json.hpp>

using json = nlohmann::json;

class Sender
{
public:
    Sender(Player_Interface* p);
    void bet_raise(chat_client *c, int amount);
    void call_check(chat_client *c);
    void fold(chat_client *c);
    void swap(chat_client *c, bool (&cards)[5]);
    void ready(chat_client *c);
private:
    Player_Interface* gamer;
};
#endif
