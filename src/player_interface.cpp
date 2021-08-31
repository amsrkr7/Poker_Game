#include "player_interface.h"
#include "card.h"
#include <iostream>
#include <vector>
#include <string>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/nil_generator.hpp>

Player_Interface::Player_Interface()
{
}

void Player_Interface::addCardToHand(Card card)
{
    Hand.push_back(card);
}
//returns the hand of the player in following format:
//"ace of hearts, king of diamonds, jack of clubs, ten of spades, queen of hearts"
//uses the toString of each Card in hand
std::vector<std::string> Player_Interface::getHand()
{

    std::vector<std::string> output;

    for(int i = 0; i < 5; i++) output.push_back(Hand[i].toString());

    return output;

}

void Player_Interface::deleteCard(int index)
{
    Hand.erase(Hand.begin()+index);
}

//returns the name of the player
std::string Player_Interface::getName()
{
    return Name;
}

//sets the name of the player with the argument
void Player_Interface::setName(std::string name)
{
    Name = name;
}

//returns the current amount of money the player has
int Player_Interface::getPlayerMoney()
{
    return Player_Money;
}

//increments the current amount of money the player has by the argument passed
void Player_Interface::addPlayerMoney(int amnt)
{
    Player_Money+=amnt;
}

//decrements the curren amount of money the player has by the argument passed
void Player_Interface::subPlayerMoney(int amnt)
{
    Player_Money-=amnt;
}

void Player_Interface::setUUID(boost::uuids::uuid newid)
{
    UUID = newid;
}

//returns the UUID of the player.
boost::uuids::uuid Player_Interface::getUUID()
{
    return UUID;
}
