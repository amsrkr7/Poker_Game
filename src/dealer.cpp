#include <iostream>
#include <boost/uuid/uuid.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <string>
#include <vector>
#include <queue>
#include <cstring>
#include "dealer.h"
#include "card.h"
#include "deck.h"
#include "chat_message.h"
#include "broadcast.h"
#include "chat_server.h"

#ifndef MAX_SIZE
#define MAX_SIZE 4096
#endif

//RULES
#define ANTE_VAL 5
#define MIN_BET 10

enum act {
    CALLED = 1,
    RAISED,
    FOLDED,
    SWAPPED
};


Dealer::Dealer(std::queue<chat_message>* Q, Broadcast* b) : message_queue{Q}, bcast{b}
{
}

//represents a full round.
//separated in 5 phases:
//  Ante : asks each player for a specific ante amount or else they fold.
//  Draw : distribute 5 cards to each player
//  Betting One : everyone bets/calls/folds
//  Swap : replace any number of cards each player would like
//  Betting Two: everyone bets/calls/folds
//At the end of the second Betting Round, the player with the best hand wins.
bool Dealer::run(std::vector<Player*> players, int striker_index)
{
    pot = 0;
    cardsDealt = false;
    Round_Order = players;
    striker = striker_index;
    has_folded = new bool[Round_Order.size()];
    int winner;

    for(unsigned int i =0; i<Round_Order.size(); i++)
    {
        last_actions.push_back("waiting");
        has_folded[i] = false;
    }

    deck.shuffle();

    // run throught the phases of the game
    this->antePhase(); // up to five players join by paying ante

    int count = 0;
    for(unsigned int i =0; i<Round_Order.size(); i++)
    {
        if(has_folded[i]==false) count++;
    }
    if(count == 0)
    {
        std::cout << "All players folded" << std::endl;
        winReceive(-1);
        return true;
    }
    
    winner = didSomeoneWin();
    if(winner!=-1)
    {
        std::cout << "winner is " << Round_Order[winner]->getName() << std::endl;
        winReceive(winner);
        return true;
    }

    this->distribute(); // dealer gives everyone 5 cards
    std::cout << "\nbetting round: " << std::endl;
    this->bettingRound(); // first betting round starts.

    winner = didSomeoneWin(); //checks if only one player has NOT folded
    if(winner!=-1)
    {
        std::cout << "\nwinner is " << Round_Order[winner]->getName() << std::endl;
        cleanCards();
        winReceive(winner);
        return true;
    }
    std::cout << "\nswapping round: " << std::endl;
    this->swapRound(); // players may swap cards if desired
    std::cout << "\nbetting round: " << std::endl;
    this->bettingRound(); // second betting round

    winner = didSomeoneWin();
    if(winner!=-1)
    {
        std::cout << "\nwinner is " << Round_Order[winner]->getName() << std::endl;
        cleanCards();
        winReceive(winner);
        return true;
    }

    winner = this->findWinner(); // returns the one with the highest card
    Round_Order[winner]->addPlayerMoney(pot);
    std::cout << "winner is " << Round_Order[winner]->getName() << std::endl;
    cleanCards();
    cardsDealt = false;
    std::cout << "cards cleaned" <<std::endl;
    winReceive(winner);
    return true;
}

//waits for all players to ante
void Dealer::anteReceive()
{
    unsigned int received = 0;
    int action[Round_Order.size()] = {0};


    while(received!=Round_Order.size())
    {
        if(!message_queue->empty())
        {
            char content_[MAX_SIZE];
            chat_message newMessage = message_queue->front();
            //copies the content of the message to a string
            std::strncpy(content_, newMessage.body(), newMessage.body_length());
            content_[newMessage.body_length()] = '\0';
            auto j = json::parse(content_);
            message_queue->pop();
            //getting which player sent it

            std::string ID_ = j["from"]["uuid"];

            char *ID = new char[ID_.size() + 1];
            std::strcpy(ID, ID_.c_str());

            boost::uuids::uuid PlayerID = boost::lexical_cast<boost::uuids::uuid>(ID);
            delete[] ID;

            Player* p_ = NULL;
            unsigned int i;
            for(i = 0; i< Round_Order.size(); i++)
            {
                if(Round_Order[i]->getUUID() == PlayerID && action[i]==0)
                {
                    p_ = Round_Order[i];
                    break;
                }
            }
            if(p_)
            {
                if(j["event"]=="checked")
                {
                    action[i] = act::CALLED;
                    received++;
                    last_actions[i] = "ante'd'!";
                    std::cout << p_->getName() << ": player ante!" << std::endl;
                }
                else if(j["event"] == "folded")
                {
                    action[i] = act::FOLDED;
                    received++;
                    last_actions[i] = "folded!";
                    std::cout << p_->getName()  << ": player folded!" << std::endl;
                }
            }

        }
    }
    for(int index = (Round_Order.size()-1); index >=0; index--)
    {
        if(action[index]== act::CALLED)
        {
            pot += ANTE_VAL;
            Round_Order[index]-> subPlayerMoney(ANTE_VAL);
        }
        else if(action[index]== act::FOLDED)
        {
            std::cout << "We have detected that you folded" << std::endl;
            has_folded[index] = true;
        }
    }
}


//waits for all players still in the game to swap
void Dealer::swapReceive()
{
    unsigned int received = 0;
    for(unsigned int i=0; i<Round_Order.size(); i++)
    {
        if(has_folded[i]) received++;
    }

    bool has_played[Round_Order.size()] = {false};

    while(received!=Round_Order.size())
    {
        if(!message_queue->empty())
        {
            char content_[MAX_SIZE];
            chat_message newMessage = message_queue->front();
            //copies the content of the message to a string
            std::strncpy(content_, newMessage.body(), newMessage.body_length());
            content_[newMessage.body_length()] = '\0';
            auto j = json::parse(content_);
            message_queue->pop();
            //getting which player sent it

            std::string ID_ = j["from"]["uuid"];

            char *ID = new char[ID_.size() + 1];
            std::strcpy(ID, ID_.c_str());

            boost::uuids::uuid PlayerID = boost::lexical_cast<boost::uuids::uuid>(ID);
            std::cout << "\nuuid : " << PlayerID << std::endl;
            delete[] ID;

            Player* p_ = NULL;
            unsigned int i;
            for(i = 0; i< Round_Order.size(); i++)
            {
                if(Round_Order[i]->getUUID() == PlayerID && has_played[i]==false && has_folded[i]==false)
                {
                    p_ = Round_Order[i];
                    break;
                }
            }
            if(p_)
            {
                if(j["event"]=="swap")
                {
                    int num = 0;
                    for(int k = 4; k>=0; k--)
                    {
                        if(j["to_swap"][k]==true)
                        {
                            num++;
                            p_->deleteCard(k);
                            p_->addCardToHand(deck.draw());
                        }
                    }
                    has_played[i] = true;
                    received++;
                    last_actions[i] = "swapped " + std::to_string(num) + " cards!";
                    std::cout << p_->getName() << ": player swapped!" << std::endl;
                }
            }

        }
    }
}


//goes through all the players and ask for them to bet
void Dealer::bettingRound()
{
    unsigned int received = 0;
    int action = 0;
    int bets[Round_Order.size()] = {0};
    int bet_amount = 0;
    int min_bet = MIN_BET;
    int index = striker;


    while(received!=Round_Order.size())
    {
        if(!has_folded[index] && Round_Order[index]->getPlayerMoney()!=0)
        {
            Player* p_ = Round_Order[index];
            std::cout << p_->getName() << ": please play!" << std::endl;
            boost::uuids::uuid p_ID = p_->getUUID();
            bcast->betting_message(this->game_json(),bet_amount, min_bet, boost::uuids::to_string(p_ID));

            action = waitForBet(p_ID, &bet_amount);
            if(action==act::RAISED)
            {
                received = 0;
                pot += bet_amount - bets[index];
                p_-> subPlayerMoney(bet_amount - bets[index]);
                bets[index] = bet_amount;
                min_bet = 2*bet_amount;
                last_actions[index] = "raised $" + std::to_string(bet_amount) + "!";
                std::cout << p_->getName() << ": player raised!" << std::endl;
            }
            else if(action==act::CALLED)
            {
                pot += bet_amount - bets[index];
                p_-> subPlayerMoney(bet_amount - bets[index]);
                bets[index] = bet_amount;
                last_actions[index] = "called!";
                std::cout << p_->getName() <<  ": player called!" << std::endl;
            }
            else if(action ==act::FOLDED)
            {
                has_folded[index] = true;
                last_actions[index] = "folded!";
                std::cout << p_->getName() << ": player folded!" << std::endl;
            }
        }
        else std::cout << Round_Order[index]->getName() << ": player already folded" << std::endl;
        if(didSomeoneWin()!=-1) return;
        index = (index+1)%Round_Order.size();
        received++;
    }
}

//wait for a SPECIFIC player to bet
int Dealer::waitForBet(boost::uuids::uuid target_player, int* bet_amount)
{
    std::cout << "bet amount orig is " << *bet_amount << std::endl;
    while(true)
    {
        if(!message_queue->empty())
        {
            char content_[MAX_SIZE];
            chat_message newMessage = message_queue->front();
            //copies the content of the message to a string
            std::strncpy(content_, newMessage.body(), newMessage.body_length());
            content_[newMessage.body_length()] = '\0';
            auto j = json::parse(content_);
            message_queue->pop();
            //getting which player sent it

            std::string ID_ = j["from"]["uuid"];

            char *ID = new char[ID_.size() + 1];
            std::strcpy(ID, ID_.c_str());

            boost::uuids::uuid PlayerID = boost::lexical_cast<boost::uuids::uuid>(ID);
            delete[] ID;

            if(PlayerID == target_player)
            {
                std::string action = j["event"];
                if(action == "folded")
                {
                    return act::FOLDED;
                }
                else if(action == "checked")
                {
                    return act::CALLED;
                }
                else if(action == "bet_raise")
                {
                    int newBet = j["bet amount"];
                    if(newBet>*bet_amount)
                    {
                        *bet_amount = newBet;
                        std::cout << "bet amount new is " << *bet_amount << std::endl;
                        return act::RAISED;
                    }
                    else
                        return act::CALLED;
                }
            }
            else
                std::cout << "wrong player played" << std::endl;
        }
    }
}
//distributes 5 cards to each player.
//two loops
void Dealer::distribute()
{

    for(unsigned int i=0; i< Round_Order.size(); i++)
    {
        for(int j=0; j<5; j++) Round_Order[i]->addCardToHand(deck.draw());
    }
    chat_message client_input;

    cardsDealt = true;
    bcast->distribute_message(this->game_json());

}

//goes through each player and compare their hand value.
//the person with the biggest one wins.
//returns the index of the winner
int Dealer::findWinner()
{
    int winner = -1;
    int max = 0;
    for(unsigned int i=0; i<Round_Order.size(); i++)
    {
        if(!has_folded[i])
        {
            int dum = Round_Order[i]->getHandValue();
            std::cout << "value is " << std::hex << dum << std::endl;
            if(dum>max)
            {
                max = dum;
                winner = i;
            }
        }
    }
    return winner;
}

//sends the message and then waits for everyone to swap
void Dealer::swapRound()
{
    bcast->swap_message(this->game_json());
    swapReceive();
}

//sends the message and then waits for everyone to ante
void Dealer::antePhase()
{
    bcast->ante_message(this->game_json(),5);
    anteReceive();
}

//waits for one player to input anything
void Dealer::winReceive(int winner)
{
    for(unsigned int i =0; i<Round_Order.size(); i++)
    {
        std::cout << "checking player " << i << std::endl;
        last_actions[i] = (i==(unsigned) winner && !has_folded[i]) ? "WON" : "LOST";
        has_folded[i] = false;
    }
    bcast->win_message(this->game_json());
    std::cout << "sent win message" << std::endl;
    while(true)
    {
        if(!message_queue->empty())
        {
            std::cout << "got new message" << std::endl;
            char content_[MAX_SIZE];
            chat_message newMessage = message_queue->front();
            //copies the content of the message to a string
            std::strncpy(content_, newMessage.body(), newMessage.body_length());
            content_[newMessage.body_length()] = '\0';
            auto j = json::parse(content_);
            message_queue->pop();
            //getting which player sent it

            
            if(j["event"]=="swap" || j["event"]=="folded" || j["event"]=="bet_raise" || j["event"]=="checked" )
            {
                return;
            }
        }
    }
}

//returns the state of the game as a json
json Dealer::game_json()
{
    //int num_players = Round_Order.size();
    json game_info;
    std::vector<std::string> names;
    std::vector<int> balance;
    std::vector<std::string> uuids;
    for(auto it : Round_Order)
    {
        names.push_back(it->getName());
        balance.push_back(it->getPlayerMoney());
        uuids.push_back(boost::uuids::to_string(it->getUUID()));
    }
    game_info["names"] = names;
    game_info["balance"] = balance;
    game_info["uuids"] = uuids;
    game_info["last actions"] = last_actions;
    if(cardsDealt)
    {
        std::vector<std::string> hands;
        for(auto it : Round_Order)
        {
            std::vector<std::string> newHand = it->getHand();
            hands.insert(hands.end(),newHand.begin(), newHand.end() );
        }
        game_info["hands"] = hands;
    }

    std::vector<bool> folded;
    for(unsigned int i=0; i< Round_Order.size(); i++)
    {
        folded.push_back(has_folded[i]);
    }
    game_info["has folded"] = folded;
    game_info["from"] = "server";
    game_info["pot"] = pot;
    return game_info;
}

//returns the index of the only player who didn't fold. returns -1 if it couldn't find them.
int Dealer::didSomeoneWin()
{
    int count = 0;
    unsigned int winner;
    for(unsigned int i = 0; i < Round_Order.size(); i++)
    {
        if(has_folded[i]==false)
        {
            winner = i;
            count++;
        }
    }
    if(count == 1)
    {
        Round_Order[winner]->addPlayerMoney(pot);
        pot = 0;
        return winner;
    }
    return -1;
}

void Dealer::cleanCards()
{
  for(unsigned int i = 0; i< Round_Order.size(); i++)
  {
    for(int k = 4; k >= 0; k--)
    {
      Round_Order[i]->deleteCard(k);
    }
  }

}
