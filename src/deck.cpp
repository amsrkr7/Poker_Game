#include "card.h"
#include "deck.h"
#include <string>
#include <vector>
#include <iostream>
#include <algorithm>
#include <ctime>
#include <chrono>
#include <random>
Deck::Deck()
{
    deck_counter = 0;
    for(char i = 0; i<4; i++)
    {
        for(char j=2; j<0x0F; j++)
        {
            char newCard = (i*0x10) +j;
            deck_.push_back(Card{newCard});
        }
    }
}

//shuffles all the cards in the Deck, and resets the deck counter
void Deck::shuffle()
{
    deck_counter = 0;
    
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();

    std::shuffle (deck_.begin(), deck_.end(), std::default_random_engine(seed));
}

//returns the card @ the deck counter, and decrements the deck counter.
Card Deck::draw()
{
    return deck_[deck_counter++];
}
