# this is an example makefile for the first homework
# assignment this semester
#Ameet Subedi, Hadrien Nguyen, Jonah Branch

CXXFLAGS+= -Wall -O0 -g -std=c++11 
CPPFLAGS+=-I./include -I./asio-1.12.2/include
LDLIBS+= -lpthread -DASIO_STANDALONE
GTKLIBS = `pkg-config --cflags --libs gtkmm-3.0`

all: pokerpp serverpp

asio-1.12.2:
	tar xzf asio-1.12.2.tar.gz

chat_server.o: include/chat_server.h include/chat_message.h src/chat_server.cpp broadcast.o player.o
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) src/chat_server.cpp $(LDLIBS) -c -o chat_server.o

serverpp: dealer.o player.o broadcast.o player_interface.o card.o chat_server.o deck.o src/dealer_driver.cpp
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) src/dealer_driver.cpp $(LDLIBS) player.o player_interface.o chat_server.o broadcast.o dealer.o card.o deck.o -o serverpp

chat_client.o: include/chat_client.h include/chat_message.h include/gui.h player_interface.o src/chat_client.cpp
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) src/chat_client.cpp $(LDLIBS) $(GTKLIBS) -c -o chat_client.o

pokerpp: gui.o card.o chat_client.o sender.o player_interface.o src/player_driver.cpp
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) src/player_driver.cpp gui.o sender.o chat_client.o card.o player_interface.o $(LDLIBS) $(GTKLIBS) -lX11 -o pokerpp

deck.o: src/deck.cpp include/deck.h card.o
	$(CXX) $(CXXFLAGS) $(CPPFLAGS)  src/deck.cpp -c -o deck.o

broadcast.o: src/broadcast.cpp include/broadcast.h
	$(CXX) $(CXXFLAGS) $(CPPFLAGS)  src/broadcast.cpp -c -o broadcast.o

dealer.o: src/dealer.cpp include/dealer.h card.o deck.o broadcast.o chat_server.o include/chat_message.h
	$(CXX) $(CXXFLAGS) $(CPPFLAGS)  src/dealer.cpp -c -o dealer.o

player.o: src/player.cpp include/player.h player_interface.o card.o
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) src/player.cpp -c -o player.o

player_interface.o: src/player_interface.cpp include/player_interface.h card.o
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) src/player_interface.cpp -c -o player_interface.o

card.o: src/card.cpp include/card.h
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) src/card.cpp -c -o card.o
	
sender.o: src/sender.cpp include/sender.h chat_client.o player_interface.o
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) src/sender.cpp -c -o sender.o
	
gui.o: asio-1.12.2 src/gui.cpp include/gui.h sender.o player_interface.o
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) src/gui.cpp -c -o gui.o $(GTKLIBS)

clean:
	-rm -rf asio-1.12.2
	-rm -f chat_client
	-rm -f chat_server
	-rm -f pokerpp	
	-rm -f serverpp
	-rm -f src/json
	-rm -f *.o

