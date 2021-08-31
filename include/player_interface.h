#ifndef PLAYER_INTERFACE_H
#define PLAYER_INTERFACE_H
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <string>
#include <vector>
#include "card.h"

class Player_Interface
{
public:
    Player_Interface();
    std::vector<std::string> getHand();
    void deleteCard(int index);
    void addCardToHand(Card card);
    std::string getName();
    void setName(std::string);
    int getPlayerMoney();
    void addPlayerMoney(int);
    void subPlayerMoney(int);
    void setUUID(boost::uuids::uuid);
    boost::uuids::uuid getUUID();

protected:
    std::vector<Card> Hand; //cards in the player's hand

private:
    std::string Name; //player name
    boost::uuids::uuid UUID;
    int Player_Money;
};

#endif
