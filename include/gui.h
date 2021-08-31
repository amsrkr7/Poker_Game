#ifndef WINDOW_H
#define WINDOW_H

#include <iostream>
#include <gtkmm.h>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <algorithm>
#include <mutex>
#include "sender.h"
#include "player_interface.h"

class Gui
{
public:
    Gui(chat_client *c, Player_Interface*);
    void BetButton_clicked();
    void CallButton_clicked();
    void FoldButton_clicked();
    void SwapButton_clicked();
    void ReadyButton_clicked();
    bool* get_toggled(bool* states);
    void set_names(std::vector<std::string> names);
    void set_cards(std::vector<std::string> player_hands);
    std::string getUnicode(char cardValue);
    void update_game_info(int pot_amount, int bet_amount);
    void set_money(std::vector<int> monies);
    void update_slider(int min, int max, int def);
    void update_labels(std::vector<std::string> last_actions);
    int get_slider();
    std::string get_player_name();

    void update(chat_message message);

private:

    chat_client *c;
    Sender* send;
    Player_Interface *gamer;
    int my_index;
    std::mutex Mex;

    Glib::RefPtr<Gtk::Application> app;
    
    // mainwin dynamic elements
    Glib::RefPtr<Gtk::Builder> Build;
    Gtk::Window* MainWin;
    Gtk::Window* NameWin;

    Gtk::Entry* NameEntry;

    Gtk::Button* BetButton;
    Gtk::Button* CallButton;
    Gtk::Button* FoldButton;
    Gtk::Button* SwapButton;
    Gtk::Button* ReadyButton;

    Gtk::ToggleButton* CardToggle1;
    Gtk::ToggleButton* CardToggle2;
    Gtk::ToggleButton* CardToggle3;
    Gtk::ToggleButton* CardToggle4;
    Gtk::ToggleButton* CardToggle5;

    Gtk::Label* LastPlay_Label;
    Gtk::Label* PotAmount_Label;
    Gtk::Label* BetAmount_Label;
    Gtk::Label* Name_Label;
    Gtk::Label* Wallet_Label;
    Gtk::Label* SliderMin_Label;
    Gtk::Label* SliderMax_Label;
    Gtk::Scale* Slider;

    Gtk::Label* Player2_Name_Label;
    Gtk::Label* Player3_Name_Label;
    Gtk::Label* Player4_Name_Label;
    Gtk::Label* Player5_Name_Label;

    Gtk::Label* Player2_Last_Action_Label;
    Gtk::Label* Player3_Last_Action_Label;
    Gtk::Label* Player4_Last_Action_Label;
    Gtk::Label* Player5_Last_Action_Label;

    Gtk::Label* Player2_Money_Label;
    Gtk::Label* Player3_Money_Label;
    Gtk::Label* Player4_Money_Label;
    Gtk::Label* Player5_Money_Label;

    Gtk::Label* Player2_Cards_Label;
    Gtk::Label* Player3_Cards_Label;
    Gtk::Label* Player4_Cards_Label;
    Gtk::Label* Player5_Cards_Label;

};

#endif
