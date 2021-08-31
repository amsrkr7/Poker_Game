#ifndef BROADCAST_H
#define BROADCAST_H

#include "chat_server.h"
#include <json.hpp>
#include <string>

using json = nlohmann::json;

class Broadcast
{
public:
    void link_room(chat_room* cs);
    bool verify();
    void ante_message(json game_info, int);
    void distribute_message(json game_info);
    void betting_message(json game_info, int bet_amount, int min, std::string uuid);
    void swap_message(json game_info);
    void win_message(json game_info);
private:
    chat_room* room;
};

#endif
