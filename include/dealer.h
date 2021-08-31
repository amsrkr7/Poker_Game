#ifndef DEALER_H
#define DEALER_H

#include <boost/uuid/uuid.hpp>
#include <string>
#include <vector>
#include <queue>
#include "player.h"
#include "card.h"
#include "deck.h"
#include "broadcast.h"
#include "chat_server.h"
#include <json.hpp>

using json = nlohmann::json;

class Dealer
{
public:
    Dealer(std::queue<chat_message>* Q, Broadcast* b);
    bool run(std::vector<Player*>,int);
    json game_json();
private:
    void antePhase();
    void anteReceive();
    void bettingRound();
    int waitForBet(boost::uuids::uuid target_player, int* bet_amount);
    void distribute();
    void swapRound();
    void swapReceive();
    void winReceive(int);
    int findWinner();
    int didSomeoneWin();
    void cleanCards();

    bool* has_folded;
    bool cardsDealt;
    std::queue<chat_message>* message_queue;
    Broadcast* bcast;
    std::vector<Player*> Round_Order; //who is still playing in the round
    Deck deck;
    int striker;  //who starts each phase
    int pot;  //the amount of money in play
    std::vector<std::string> last_actions;
};

#endif
