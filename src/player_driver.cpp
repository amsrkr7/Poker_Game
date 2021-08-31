#include <cstdlib>
#include <iostream>
#include <thread>

#include "dealer.h"
#include "gui.h"
#include "chat_client.h"
#include "player_interface.h"
#include <X11/Xlib.h>
int main(int argc, char* argv[])
{
    XInitThreads();
    try
    {
        if (argc != 3)
        {
            std::cerr << "Usage: chat_client <host> <port>\n";
            return 1;
        }

        asio::io_context io_context;
        Player_Interface player;

        tcp::resolver resolver(io_context);
        auto endpoints = resolver.resolve(argv[1], argv[2]);
        chat_client c(io_context, endpoints, &player);

        std::thread t([&io_context]() {
            io_context.run();
        });

        // Gui constructor takes control of main loop. When the Gui closes, c.close() happens
        Gui my_gui{&c, &player};
        c.close();
        t.join();
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}
