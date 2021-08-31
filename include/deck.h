#ifndef DECK_H
#define DECK_H


#include "card.h"
#include <string>
#include <vector>
class Deck
{
public:
    Deck();
    void shuffle();
    Card draw();

private:
    std::vector<Card> deck_;
    int deck_counter; //where the dealer is currently in the deck
};

#endif
