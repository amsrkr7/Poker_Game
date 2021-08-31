#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <boost/uuid/uuid.hpp>
#include "player.h"
#include "player_interface.h"
#include "card.h"

Player::Player(boost::uuids::uuid uuid)
{
    this->setUUID(uuid);
}


//returns the hand value of the current hand
//  rank is gonna be the first half of the 2nd byte. (0x00A00000)
//    high card = 1
//    pair = 2
//    two pairs = 3
//    three of a kind = 4
//    straight = 5
//    flush = 6
//    full house = 7
//    four of a kind = 8
//    straight flush = 9
//  all the other cards will then follow in order
//    King = D
//    Queen = C
//    Jack = B
//    10 = A
//    Ace = E
//for example:
//  a hand: K, K, 10, 2, 7 (pair of kings)
//  should return: 0x002DA720
//  a hand: 6, 3, 4, 5, 7 (straight)
//  should return: 0x00570000
//  a hand: 8, 8, 8, Q, Q (full house)
//  should return: 0x007C8000
//  a hand = A, K, Q, J, 10, all same suit (royal flush)
//  should return: 0x009E0000
int Player::getHandValue()
{

    std::vector <char> rankVector;
    for (int i = 0; i< 5; i++)
    {
        rankVector.push_back(Hand[i].getRank());
    }
    std::sort(rankVector.begin(),rankVector.end());
    std::reverse(rankVector.begin(), rankVector.end());


    if (testForStraightFlush())
    {

        char val =rankVector[0];
        char ret = (0x90 | val);

        int value = ((int) ret) << 16;
        return value;
    }

    else if (testForFour())
    {
        if (rankVector[1] != rankVector[4])
        {
            char val =rankVector[0];
            char ret = (0x80 | val);
            int value1 = ((int) ret ) <<16;
            int value2 = ((int) rankVector[4]) << 12;
            int value = value1 | value2;
            return value;
        }

        if (rankVector[1] == rankVector[4])
        {
            char val =rankVector[1];
            char ret = (0x80 | val);
            int value1 = ((int) ret ) <<16;
            int value2 = ((int) rankVector[4]) << 12;
            int value = value1 | value2;
            return value;
        }

    }


    else if (testForHouse())
    {
        if (rankVector[0] == rankVector[2])
        {
            char val = rankVector[0];
            char ret = (0x70 | val);
            int value1 = ((int) ret ) << 16;
            int value2 = ((int) rankVector[3]) << 12;
            int value = value1 | value2;
            return value;
        }

        else
        {
            char val = rankVector[2];
            char ret = (0x70 | val);
            int value1 = ((int) ret ) << 16;
            int value2 = ((int) rankVector[0]) << 12;
            int value = value1 | value2;
            return value;
        }


    }

    else if (testForFlush())
    {
        char val1 = rankVector[0];
        char ret1 = (0x60 | val1);
        int value1 = ((int) ret1) << 16;
        char val2 = rankVector[1];
        int value2 = ((int) val2 ) << 12;
        char val3 = rankVector[2];
        int value3 = ((int) val3) << 8;
        char val4 = rankVector[3];
        int value4 = ((int) val4)<<4;
        char val5 = rankVector[4];
        int value5 = ((int) val5)<<0;
        int value =  value1 | value2 | value3| value4 | value5;

        return value;
    }



    else if (testForStraight())
    {
        char val = rankVector[0];
        char ret = (0x50 | val);

        int value = ((int) ret) <<16;
        return value;


    }

    else if (testForThree())
    {
        if (rankVector[0] == rankVector[2])
        {
            char val = (rankVector[0]);
            char ret = (0x40 | val);
            int value1 = ((int) ret) << 16;
            char val2 = rankVector[3];
            int value2 = ((int) val2)<<12;
            char val3 = rankVector[4];
            int value3 = ((int) val3 << 8);

            return value1 | value2 | value3;
        }
        else if (rankVector[1] == rankVector[3])
        {
            char val = (rankVector[1]);
            char ret = (0x40 | val);
            int value1 = ((int) ret) << 16;
            char val2 = rankVector[0];
            int value2 = ((int) val2)<<12;
            char val3 = rankVector[4];
            int value3 = ((int) val3 << 8);

            return value1 | value2 | value3;

        }
        else std::cout << "ERROR THREE OF A KIND" << std::endl;
    }
    else if (testForTwoPairs())
    {
        if ( rankVector[0] == rankVector[1]  &&  rankVector[2] == rankVector[3])
        {
            char val1 = rankVector[0];
            char ret = (0x30 | val1);
            int value1 = ((int) ret )<< 16;
            char val2 = rankVector[2];
            char value2 = ((int) val2) << 12;
            char val3 = rankVector[4];
            char value3 = ((int) val3)  <<8;

            return value1 | value2 | value3;
        }
        else if (rankVector[1] == rankVector[2] && rankVector[3] == rankVector[4])
        {
            char val1 = rankVector[1];
            char ret = (0x30 | val1);
            int value1 = ((int) ret)<< 16;
            char val2 = rankVector[3];
            int value2 = ((int) val2) << 12;
            char val3 = rankVector[0];
            int value3 = ((int) val3) << 8;

            return value1 | value2 | value3;
        }

        else
        {
            char val1 = rankVector[0];
            char ret = (0x30 | val1);
            int value1 = ((int) ret)<< 16;
            char val2 = rankVector[2];
            int value2 = ((int) val2) << 12;
            char val3 = rankVector[3];
            int value3 = ((int) val3) << 8;

            return value1 | value2 | value3;
        }
    }

    else if (testForPair())
    {
        if (rankVector[0] == rankVector[1])
        {
            char val1 = rankVector[0];
            char ret = (0x20 | val1);
            int value1 = ((int) ret)<< 16;
            char val2 = rankVector[2];
            int value2 = ((int) val2) << 12;
            char val3 = rankVector[3];
            int value3 = ((int) val3) << 8;
            char val4 = rankVector[4];
            int value4 = ((int) val4) << 4;

            return value1 | value2 | value3 | value4;
        }

        else if (rankVector[1] == rankVector[2])
        {
            char val1 = rankVector[1];
            char ret = (0x20 | val1);
            int value1 = ((int) ret)<< 16;
            char val2 = rankVector[0];
            int value2 = ((int) val2) << 12;
            char val3 = rankVector[3];
            int value3 = ((int) val3) << 8;
            char val4 = rankVector[4];
            int value4 = ((int) val4) << 4;

            return value1 | value2 | value3 | value4;
        }

        else if (rankVector[2] == rankVector[3])
        {
            char val1 = rankVector[2];
            char ret = (0x20 | val1);
            int value1 = ((int) ret)<< 16;
            char val2 = rankVector[0];
            int value2 = ((int) val2) << 12;
            char val3 = rankVector[1];
            int value3 = ((int) val3) << 8;
            char val4 = rankVector[4];
            int value4 = ((int) val4) << 4;

            return value1 | value2 | value3 | value4;
        }

        else
        {
            char val1 = rankVector[4];
            char ret = (0x20 | val1);
            int value1 = ((int) ret)<< 16;
            char val2 = rankVector[0];
            int value2 = ((int) val2) << 12;
            char val3 = rankVector[1];
            int value3 = ((int) val3) << 8;
            char val4 = rankVector[2];
            int value4 = ((int) val4) << 4;

            return value1 | value2 | value3 | value4;
        }
    }

    else
    {
        char val1 = rankVector[0];
        char ret1 = (0x10 | val1);
        int value1 = ((int) ret1) << 16;
        char val2 = rankVector[1];
        int value2 = ((int) val2 ) << 12;
        char val3 = rankVector[2];
        int value3 = ((int) val3) << 8;
        char val4 = rankVector[3];
        int value4 = ((int) val4)<<4;
        char val5 = rankVector[4];
        int value5 = ((int) val5)<<0;
        int value =  value1 | value2 | value3| value4 | value5;

        return value;
    }

    return 1;
}

//returns T if all the cards in the hands are in the same suit and follow one another
//note: Ace is either low or high, but not both
//can just call testForStraight() and testForFlush()
bool Player::testForStraightFlush()
{

    return testForStraight() && testForFlush();
}

//returns T if there are 4 cards with the same value
bool Player::testForFour()
{
    char temp = Hand[0].getRank();
    int count = 1;
    for(int i = 1; i<5; i++)
    {
        if(temp == Hand[i].getRank()) count++;
    }
    if(count==4)return true;

    temp = Hand[1].getRank();
    for(int i = 2; i<5; i++)
    {
        if(temp == Hand[i].getRank()) count++;
    }
    return count==4;
}

//returns T if there are 3 cards that are the same and 2 other cards that are the same
bool Player::testForHouse()
{
    int type1 = 1;
    int type2 = 0;
    char temp1 = Hand[0].getRank();
    char temp2 = 0;
    for(int i = 1; i<5; i++)
    {
        if(Hand[i].getRank()==temp1) type1++;
        else if(Hand[i].getRank()==temp2) type2++;
        else if(!temp2) temp2 = Hand[i].getRank();
        else return false;
    }
    if(type1<4 && type1>1 && type2<4 && type2>1) return true;
    return false;
}

//returns T if all the cards are in the same suit
bool Player::testForFlush()
{
    char suit = Hand[0].getSuit();
    for(int i = 1; i<5; i++)
    {
        if(suit!=Hand[i].getSuit()) return false;
    }
    return true;
}

//returns T if all the cards follow one another
//note: Ace is either high or low, but not both
bool Player::testForStraight()
{
    char cards[5];
    char min=99;
    int ace = -1;
    for(int i=0; i<5; i++)
    {
        cards[i] = Hand[i].getRank();
        if(cards[i] == 0x0E) ace = i;
        if(cards[i]<min) min=cards[i];
    }
    if(ace!=-1 && min < 10) cards[ace]=min=1;

    bool straight[5] = {false};
    for(int i=0; i<5; i++)
    {
        int diff = cards[i]-min;
        if(diff > 5) return false;
        if(straight[diff]) return false;
        straight[diff] = true;
    }
    return false;
}

//returns T if there are 3 cards that are the same
bool Player::testForThree()
{
    std::vector <char> rankVector;

    for (int i = 0; i< 5; i++)
    {
        rankVector.push_back(Hand[i].getRank());
    }
    std::sort(rankVector.begin(),rankVector.end());

    bool type1, type2,type3;

    if ( testForFour() || testForHouse())
    {
        return false; // infact the hand is better than 3 of a kind
    }

    type1 = rankVector[0] == rankVector[1] &&
            rankVector[1] == rankVector[2];

    type2 = rankVector[1] == rankVector[2] &&
            rankVector[2] == rankVector[3];

    type3 =  rankVector[2] == rankVector[3]&&
             rankVector[3] == rankVector[4];

    return (type1 || type2 || type3);
}

//returns T if there are two instances of the same card twice
//example: K, K, 10, 10, 2
bool Player::testForTwoPairs()
{

    std::vector <char> rankVector;

    for (int i = 0; i< 5; i++)
    {
        rankVector.push_back(Hand[i].getRank());
    }

    std::sort(rankVector.begin(),rankVector.end());

    bool type1, type2,type3;

    if ( testForFour() || testForHouse() || testForStraight())
    {
        return false; // infact the hand is better than two two pairs
    }

    type1 = rankVector[0] == rankVector[1] &&
            rankVector[2] == rankVector[3];

    type2 = rankVector[0] == rankVector[1] &&
            rankVector[3] == rankVector[4];

    type3 =  rankVector[1] == rankVector[2]&&
             rankVector[3] == rankVector[4];

    return (type1 || type2 || type3);
}

//returns T if there are 2 cards that are the same.
bool Player::testForPair()
{
    std::vector <char> rankVector;

    for (int i = 0; i< 5; i++)
    {
        rankVector.push_back(Hand[i].getRank());
    }
    std::sort(rankVector.begin(),rankVector.end());

    bool type1, type2,type3,type4;

    if ( testForFour() || testForHouse() || testForStraight() || testForTwoPairs())
    {
        return false; // infact the hand is better than a two pair
    }

    type1 = rankVector[0] == rankVector[1];
    type2 = rankVector[1] == rankVector[2];

    type3 = rankVector[2] == rankVector[3];
    type4 = rankVector[3] == rankVector[4];
    return (type1 || type2 || type3 || type4);
}
