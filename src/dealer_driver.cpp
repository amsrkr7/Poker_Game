#include <cstdlib>
#include <deque>
#include <queue>
#include <iostream>
#include <list>
#include <memory>
#include <set>
#include <utility>
#include "asio.hpp"
#include "chat_message.h"
#include "dealer.h"
#include "player.h"
#include "player_interface.h"
#include "deck.h"
#include "broadcast.h"
#include "chat_server.h"
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>

#ifndef MAX_SIZE
#define MAX_SIZE 4096
#endif

#define ANTE_VAL 5
#define START_BALANCE 100
using asio::ip::tcp;

int main(int argc, char* argv[])
{
    try
    {
        if (argc < 2)
        {
            std::cerr << "Usage: chat_server <port> [<port> ...]\n";
            return 1;
        }

        std::queue<chat_message> message_Q;
        Broadcast bcast;
        Dealer deal{&message_Q, &bcast};
        std::vector<Player*> players_;

        asio::io_context io_context;

        std::list<chat_server> servers;
        for (int i = 1; i < argc; ++i)
        {
            tcp::endpoint endpoint(tcp::v4(), std::atoi(argv[i]));
            servers.emplace_back(io_context, endpoint, &message_Q, &bcast, &players_);
        }

        std::thread t([&io_context]() {
            io_context.run();
        });
        //io_context.run();
        while(players_.size()<2)
        {
            while(players_.size()<2)
            {
                int i = 1+1;
                i+=3;
            }
        }
        
        unsigned int count;
        while(count != players_.size() || count ==5)
        {
            if(!message_Q.empty())
            {
                char content_[MAX_SIZE];
                chat_message newMessage = message_Q.front();
                //copies the content of the message to a string
                std::strncpy(content_, newMessage.body(), newMessage.body_length());
                content_[newMessage.body_length()] = '\0';
                auto j = json::parse(content_);
                message_Q.pop();
                //getting which player sent it
                
                if(j["event"] == "player_join")
                {
                    std::string ID_ = j["from"]["uuid"];
                    for(unsigned int i = 0; i < players_.size(); i++)
                    {
                        if(boost::uuids::to_string(players_[i]->getUUID()) == ID_)
                        {
                            players_[i]->setName(j["from"]["name"]);
                            count++;
                        }  
                    }
                }
            
            }
        }
               
        
        for(auto it : players_)
        {
            it->addPlayerMoney(START_BALANCE);
        }

        int striker = 0;
        while(true)
        {
            std::cout<<"\n\n" << players_[striker]->getName() << " is the striker \n" << std::endl;
            deal.run(players_, striker);
            std::cout << "\nEND of Round\n" << std::endl;
            for(int i = players_.size()-1; i>=0; i--)
            {
                if(players_.at(i)->getPlayerMoney() < ANTE_VAL)
                {
                    std::cout<<"\n\n" << players_[0]->getName() << " LOST" << std::endl;
                    players_.erase(players_.begin() + i);
                    if(i<=striker) striker--;
                }
            }
            if(players_.size()==1) break;
            striker = (striker + 1)%players_.size();
        }

        std::cout<<"\n\nCONGRATS TO " << players_[0]->getName() + " FOR WINNING THE GAME" << std::endl;
        std::cout << "GAME IS OVER" << std::endl;
        t.join();

    }
    catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}
