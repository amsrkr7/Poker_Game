#ifndef CARD_H
#define CARD_H
#include <string>

/* NOTE
   cards will have a numerical value and a string representation.
   the numerical value will be 1 byte, the first half being the suit, and the second being the number
   2 - 2          spades = 0
   3 - 3          hearts = 1
   4 - 4          clubs = 2
   5 - 5          diamonds = 3
   6 - 6
   7 - 7
   8 - 8
   9 - 9
   10 - A
   jack - B
   queen - C
   king - D
   ace - E
*/

class Card
{
public:
    Card(char);
    Card(std::string);
    std::string toString();
    char getCardValue();
    char getSuit();
    char getRank();
private:
    char card_value;  //example: 0x1E
    std::string card_name;  //example: "ace of hearts"
};

#endif
