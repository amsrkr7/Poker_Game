#ifndef PLAYER_H
#define PLAYER_H
#include <boost/uuid/uuid.hpp>
#include <string>
#include "card.h"
#include "player_interface.h"

class Player : public Player_Interface
{
public:
    Player(boost::uuids::uuid);
    int getHandValue();

private:
    bool testForStraightFlush();
    bool testForFour();
    bool testForHouse();
    bool testForFlush();
    bool testForStraight();
    bool testForThree();
    bool testForTwoPairs();
    bool testForPair();
};

#endif
