#include <string>
#include <iostream>
#include <sstream>
#include "card.h"

//instantialize the Card based on a numerical value
Card::Card(char value) : card_value{value}
{
    char rank = value & 0x0F;
    char suit = (value & 0xF0) >> 4;
    switch(rank)
    {
    case 2:
        card_name = "two";
        break;
    case 3:
        card_name = "three";
        break;
    case 4:
        card_name = "four";
        break;
    case 5:
        card_name = "five";
        break;
    case 6:
        card_name = "six";
        break;
    case 7:
        card_name = "seven";
        break;
    case 8:
        card_name = "eight";
        break;
    case 9:
        card_name = "nine";
        break;
    case 10:
        card_name = "ten";
        break;
    case 11:
        card_name = "jack";
        break;
    case 12:
        card_name = "queen";
        break;
    case 13:
        card_name = "king";
        break;
    case 14:
        card_name = "ace";
        break;
    default:
        card_name = "xxx";
        break;
    }
    card_name += " of ";
    switch(suit)
    {
    case 0:
        card_name += "spades";
        break;
    case 1:
        card_name += "hearts";
        break;
    case 2:
        card_name += "clubs";
        break;
    case 3:
        card_name += "diamonds";
        break;
    default:
        card_name = "xxx";
        break;
    }
}

//instantialize the Card based on a string
Card::Card(std::string name) : card_name{name}
{
    std::stringstream str(name);
    std::string rank, suit, dummy;
    str >> rank >> dummy >> suit;

    if(suit == "spades")
        card_value = 0x00;
    else if(suit == "hearts")
        card_value = 0x10;
    else if(suit == "clubs")
        card_value = 0x20;
    else if(suit == "diamonds")
        card_value = 0x30;
    else
        card_value = 0xF0;

    if(rank == "two")
        card_value += 0x02;
    else if(rank == "three")
        card_value += 0x03;
    else if(rank == "four")
        card_value += 0x04;
    else if(rank == "five")
        card_value += 0x05;
    else if(rank == "six")
        card_value += 0x06;
    else if(rank == "seven")
        card_value += 0x07;
    else if(rank == "eight")
        card_value += 0x08;
    else if(rank == "nine")
        card_value += 0x09;
    else if(rank == "ten")
        card_value += 0x0A;
    else if(rank == "jack")
        card_value += 0x0B;
    else if(rank == "queen")
        card_value += 0x0C;
    else if(rank == "king")
        card_value += 0x0D;
    else if(rank == "ace")
        card_value += 0x0E;
    else
        card_value = 0xFF;

}

//returns the card_name
std::string Card::toString()
{
    return card_name;
}

//returns the card_value
char Card::getCardValue()
{
    return card_value;
}

//returns the suit ()
char Card::getSuit()
{
    return card_value & 0xF0;
}

//returns the rank of the card
char Card::getRank()
{
    return card_value & 0x0F;
}
