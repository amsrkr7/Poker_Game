#include "player_interface.h"
#include "gui.h"
#include "card.h"
//links the Gtk builder to the glade XML file.
Gui::Gui(chat_client *_c, Player_Interface* g) : c{_c}, gamer{g}
{
    send = new Sender{gamer};
    //
    gdk_threads_init();
    this->app = Gtk::Application::create("poker.client", Gio::APPLICATION_NON_UNIQUE);

    this->Build = Gtk::Builder::create_from_file("./poker.glade");

    this->Build->get_widget("namewin", this->NameWin);

    this->Build->get_widget("name", this->NameEntry);
    this->Build->get_widget("READY_BUTTON", this->ReadyButton);

    this->Build->get_widget("mainwin", this->MainWin);
    // initialize Gtk::Window* NameWin;

    Build->get_widget("BET_BUTTON", this->BetButton);
    Build->get_widget("CALL_BUTTON", this->CallButton);
    Build->get_widget("FOLD_BUTTON", this->FoldButton);
    Build->get_widget("SWAP_BUTTON", this->SwapButton);
    Build->get_widget("READY_BUTTON", this->ReadyButton);

    Build->get_widget("CARD_1_TOGGLE", this->CardToggle1);
    Build->get_widget("CARD_2_TOGGLE", this->CardToggle2);
    Build->get_widget("CARD_3_TOGGLE", this->CardToggle3);
    Build->get_widget("CARD_4_TOGGLE", this->CardToggle4);
    Build->get_widget("CARD_5_TOGGLE", this->CardToggle5);

    Build->get_widget("LAST_PLAY_LABEL", this->LastPlay_Label);
    Build->get_widget("POT_AMOUNT_LABEL", this->PotAmount_Label);
    Build->get_widget("LAST_BET_LABEL", this->BetAmount_Label);
    // initialize Gtk::Label* BetAmount_Label; BET AMOUNT LABEL DOESN'T EXIST IN THE GLADE YET... NOT NEEDED???
    Build->get_widget("PLAYER_NAME_LABEL", this->Name_Label);
    Build->get_widget("PLAYER_WALLET_LABEL", this->Wallet_Label);
    Build->get_widget("SLIDER", this->Slider);
    Build->get_widget("SLIDER_MIN", this->SliderMin_Label);
    Build->get_widget("SLIDER_MAX", this->SliderMax_Label);

    Build->get_widget("PLAYER_2_NAME", this->Player2_Name_Label);
    Build->get_widget("PLAYER_3_NAME", this->Player3_Name_Label);
    Build->get_widget("PLAYER_4_NAME", this->Player4_Name_Label);
    Build->get_widget("PLAYER_5_NAME", this->Player5_Name_Label);

    Build->get_widget("PLAYER_2_LAST_ACTION", this->Player2_Last_Action_Label);
    Build->get_widget("PLAYER_3_LAST_ACTION", this->Player3_Last_Action_Label);
    Build->get_widget("PLAYER_4_LAST_ACTION", this->Player4_Last_Action_Label);
    Build->get_widget("PLAYER_5_LAST_ACTION", this->Player5_Last_Action_Label);

    Build->get_widget("PLAYER_2_MONEY", this->Player2_Money_Label);
    Build->get_widget("PLAYER_3_MONEY", this->Player3_Money_Label);
    Build->get_widget("PLAYER_4_MONEY", this->Player4_Money_Label);
    Build->get_widget("PLAYER_5_MONEY", this->Player5_Money_Label);

    Build->get_widget("PLAYER_2_CARDS", this->Player2_Cards_Label);
    Build->get_widget("PLAYER_3_CARDS", this->Player3_Cards_Label);
    Build->get_widget("PLAYER_4_CARDS", this->Player4_Cards_Label);
    Build->get_widget("PLAYER_5_CARDS", this->Player5_Cards_Label);

    //this->Player5_Name_Label->set_text("🂡🂡🂡🂡🂡");

    if(this->BetButton)
    {
        this->BetButton->signal_clicked().connect( [this] () {
            this->BetButton_clicked();
        } );
    }

    if(this->CallButton)
    {
        this->CallButton->signal_clicked().connect( [this] () {
            this->CallButton_clicked();
        });
    }

    if(this->FoldButton)
    {
        this->FoldButton->signal_clicked().connect( [this] () {
            this->FoldButton_clicked();
        });
    }

    if(this->SwapButton)
    {
        this->SwapButton->signal_clicked().connect( [this] () {
            this->SwapButton_clicked();
        });
    }

    if(this->ReadyButton)
    {
        this->ReadyButton->signal_clicked().connect( [this] () {
            this->ReadyButton_clicked();
        });
    }

    this->update_slider(0, 100, 20);


    c->link_gui(this);
    this->app->run(*this->NameWin);
    //app->run(*this->MainWin);

}

void Gui::ReadyButton_clicked()
{
    gamer->setName(this->NameEntry->get_text());
    send->ready(this->c);
    this->app->add_window(*this->MainWin);
    this->MainWin->show();
    this->NameWin->hide();
    //this->NameWin->close();
}

//sends out a message to the server: the bet amount and all the player info.
void Gui::BetButton_clicked()
{
    //std::cout << "Bet button was pressed" << std::endl;

    send->bet_raise(this->c, this->get_slider());
}

//sends out all the player info + action name
void Gui::CallButton_clicked()
{
    //std::cout << "Call button was pressed" << std::endl;

    this->CallButton->set_sensitive(false);
    this->FoldButton->set_sensitive(false);

    send->call_check(this->c);
}

//sends out player info + action name
void Gui::FoldButton_clicked()
{
    //std::cout << "Fold button was pressed" << std::endl;

    this->CallButton->set_sensitive(false);
    this->FoldButton->set_sensitive(false);
    send->fold(this->c);
}

//sends out which cards were selected
void Gui::SwapButton_clicked()
{
    //std::cout << "Swap button was pressed" << std::endl;
    bool states[5];

    states[0] = this->CardToggle1->get_active();
    states[1] = this->CardToggle2->get_active();
    states[2] = this->CardToggle3->get_active();
    states[3] = this->CardToggle4->get_active();
    states[4] = this->CardToggle5->get_active();

    this->CardToggle1->set_active(false);
    this->CardToggle2->set_active(false);
    this->CardToggle3->set_active(false);
    this->CardToggle4->set_active(false);
    this->CardToggle5->set_active(false);

    this->SwapButton->set_sensitive(false);

    send->swap(this->c, states);
}

// a function which returns which toggle buttons (cards) are selected by the user
// must pass in the address to a bool array
// *** THIS FUNCTION IS NOT WORKING, WILL SEG FAULT!!! ***
bool* Gui::get_toggled(bool * states)
{
    states[0] = CardToggle1->get_active();
    states[1] = CardToggle2->get_active();
    states[2] = CardToggle3->get_active();
    states[3] = CardToggle4->get_active();
    states[4] = CardToggle5->get_active();

    return states;
}


// a function which sets the names to be displayed on the GUI. 'name1' is the username for the player
void Gui::set_names(std::vector<std::string> names)
{
    this->Name_Label->set_text(names.at(0));

    if(names.size() > 1) this->Player2_Name_Label->set_text(names.at(1));
    else this->Player2_Name_Label->set_text(" ");
    if(names.size() > 2) this->Player3_Name_Label->set_text(names.at(2));
    else this->Player3_Name_Label->set_text(" ");
    if(names.size() > 3) this->Player4_Name_Label->set_text(names.at(3));
    else this->Player4_Name_Label->set_text(" ");
    if(names.size() > 4) this->Player5_Name_Label->set_text(names.at(4));
    else this->Player5_Name_Label->set_text(" ");

}

void Gui::set_money(std::vector<int> monies)
{
    this->Wallet_Label->set_text(std::to_string(monies.at(0)));

    if(monies.size() > 1) this->Player2_Money_Label->set_text(std::to_string(monies.at(1)));
    else this->Player2_Money_Label->set_text(" ");
    if(monies.size() > 2) this->Player3_Money_Label->set_text(std::to_string(monies.at(2)));
    else this->Player3_Money_Label->set_text(" ");
    if(monies.size() > 3) this->Player4_Money_Label->set_text(std::to_string(monies.at(3)));
    else this->Player4_Money_Label->set_text(" ");
    if(monies.size() > 4) this->Player5_Money_Label->set_text(std::to_string(monies.at(4)));
    else this->Player5_Money_Label->set_text(" ");
    std::cout << "money set" << std::endl;
}

/*
void Gui::set_cards(std::vector<std::vector<std::string>> player_hands)
{

  this->CardToggle1->set_label(player_hands.at(0).at(0));
  this->CardToggle2->set_label(player_hands.at(0).at(1));
  this->CardToggle3->set_label(player_hands.at(0).at(2));
  this->CardToggle4->set_label(player_hands.at(0).at(3));
  this->CardToggle5->set_label(player_hands.at(0).at(4));

}
*/

//"seven of hearts", "ace of spades"
void Gui::set_cards(std::vector<std::string> player_hands)
{
    int num_players = player_hands.size() / 5;

    std::cout << "cards setting" << std::endl;

    if(num_players > 0) {
        Card card1{player_hands[0]};
        std::string c1 = getUnicode(card1.getCardValue());

        std::cout << "cards unicode 1 is " + c1 << std::endl;
        this->CardToggle1->set_label(c1);

        Card card2{player_hands[1]};
        std::string c2 = getUnicode(card2.getCardValue());
        std::cout << "cards unicode 2 is " + c2 << std::endl;
        this->CardToggle2->set_label(c2);

        Card card3{player_hands[2]};
        std::string c3 = getUnicode(card3.getCardValue());
        std::cout << "cards unicode 3 is " + c3 << std::endl;
        this->CardToggle3->set_label(c3);

        Card card4{player_hands[3]};
        std::string c4 = getUnicode(card4.getCardValue());

        std::cout << "cards unicode 4 is " + c4 << std::endl;
        this->CardToggle4->set_label(c4);

        Card card5{player_hands[4]};
        std::cout << "card generating 5" << std::endl;
        std::string c5 = getUnicode(card5.getCardValue());
        std::cout << "cards unicode 5 is " + c5 << std::endl;
        this->CardToggle5->set_label(c5);
        std::cout << "cards setting 5" << std::endl;
    } else {

        this->CardToggle1->set_label(" ");
        this->CardToggle2->set_label(" ");
        this->CardToggle3->set_label(" ");
        this->CardToggle4->set_label(" ");
        this->CardToggle5->set_label(" ");
    }

    if(num_players > 1)
        this->Player2_Cards_Label->set_text("🂠🂠🂠🂠🂠");
    else this->Player2_Cards_Label->set_text(" ");
    if(num_players > 2)
        this->Player3_Cards_Label->set_text("🂠🂠🂠🂠🂠");
    else this->Player3_Cards_Label->set_text(" ");
    if(num_players > 3)
        this->Player4_Cards_Label->set_text("🂠🂠🂠🂠🂠");
    else this->Player4_Cards_Label->set_text(" ");
    if(num_players > 4)
        this->Player5_Cards_Label->set_text("🂠🂠🂠🂠🂠");
    else this->Player5_Cards_Label->set_text(" ");
}

std::string Gui::getUnicode(char cardValue)
{
    if (cardValue == 0x02) return u8"🂢";
    else if (cardValue == 0x03) return u8"🂣";
    else if (cardValue == 0x04) return u8"🂤";
    else if (cardValue == 0x05) return u8"🂥";
    else if (cardValue == 0x06) return u8"🂦";
    else if (cardValue == 0x07) return u8"🂧";
    else if (cardValue == 0x08) return u8"🂨";
    else if (cardValue == 0x09) return u8"🂩";
    else if (cardValue == 0x0A) return u8"🂪";
    else if (cardValue == 0x0B) return u8"🂫";
    else if (cardValue == 0x0C) return u8"🂭";
    else if (cardValue == 0x0D) return u8"🂮";
    else if (cardValue == 0x0E) return u8"🂡";

    else if (cardValue == 0x12) return u8"🂲";
    else if (cardValue == 0x13) return u8"🂳";
    else if (cardValue == 0x14) return u8"🂴";
    else if (cardValue == 0x15) return u8"🂵";
    else if (cardValue == 0x16) return u8"🂶";
    else if (cardValue == 0x17) return u8"🂷";
    else if (cardValue == 0x18) return u8"🂸";
    else if (cardValue == 0x19) return u8"🂹";
    else if (cardValue == 0x1A) return u8"🂺";
    else if (cardValue == 0x1B) return u8"🂻";
    else if (cardValue == 0x1C) return u8"🂽";
    else if (cardValue == 0x1D) return u8"🂾";
    else if (cardValue == 0x1E) return u8"🂱";

    else if (cardValue == 0x22) return u8"🃒";
    else if (cardValue == 0x23) return u8"🃓 ";
    else if (cardValue == 0x24) return u8"🃔";
    else if (cardValue == 0x25) return u8"🃕";
    else if (cardValue == 0x26) return u8"🃖";
    else if (cardValue == 0x27) return u8"🃗";
    else if (cardValue == 0x28) return u8"🃘";
    else if (cardValue == 0x29) return u8"🃙";
    else if (cardValue == 0x2A) return u8"🃚";
    else if (cardValue == 0x2B) return u8"🃛";
    else if (cardValue == 0x2C) return u8"🃝";
    else if (cardValue == 0x2D) return u8"🃞";
    else if (cardValue == 0x2E) return u8"🃑";



    else if (cardValue == 0x32) return u8"🃂";
    else if (cardValue == 0x33) return u8"🃃";
    else if (cardValue == 0x34) return u8"🃄";
    else if (cardValue == 0x35) return u8"🃅";
    else if (cardValue == 0x36) return u8"🃆";
    else if (cardValue == 0x37) return u8"🃇";
    else if (cardValue == 0x38) return u8"🃈";
    else if (cardValue == 0x39) return u8"🃉";
    else if (cardValue == 0x3A) return u8"🃊";
    else if (cardValue == 0x3B) return u8"🃋";
    else if (cardValue == 0x3C) return u8"🃍";
    else if (cardValue == 0x3D) return u8"🃎";
    else return u8"🃁";


}

// this function updates the game info with the latest pot amount and last bet amount
void Gui::update_game_info(int pot_amount, int bet_amount)
{
    std::cout << "chanign pot anda ll" << std::endl;
    this->PotAmount_Label->set_text(std::to_string(pot_amount));
    this->BetAmount_Label->set_text(std::to_string(bet_amount));
}

// this function updates the label displaying how much money the player has.
// player name is updated through the set_names function seen above

// this updates the slider info for the player to use. Sets the min and max positions of the slider, as well as the default position
// also updates the labels for the slider
void Gui::update_slider(int min, int max, int def = -1)
{

    if(def == -1) def = min; // if default wasn't assigned, set it to whatever min was

    if(max < min*2) this->Slider->set_sensitive(false);
    else this->Slider->set_sensitive(true);

    this->SliderMin_Label->set_text(std::to_string(min));
    this->SliderMax_Label->set_text(std::to_string(max));

    this->Slider->set_range((double) min, (double) max);
    this->Slider->set_value((double) min);

    std::cout << "updated slider" << std::endl;
}

// this returns the slider position
int Gui::get_slider()
{
    std::cout << "slider value is " << Slider->get_value() << std::endl;
    return Slider->get_value();
}

// this function opens up the ready window (NameWin) , prompting the user for their name
// the return value is the name
std::string Gui::get_player_name()
{
    return "";
}

void Gui::update_labels(std::vector<std::string> last_actions)
{
    std::cout << "updating labels" << std::endl;
    this->LastPlay_Label->set_text(last_actions.at(0));

    if(last_actions.size() > 1)
        this->Player2_Last_Action_Label->set_text(last_actions.at(1));
    else this->Player2_Last_Action_Label->set_text(" ");
    if(last_actions.size() > 2)
        this->Player3_Last_Action_Label->set_text(last_actions.at(2));
    else this->Player3_Last_Action_Label->set_text(" ");
    if(last_actions.size() > 3)
        this->Player4_Last_Action_Label->set_text(last_actions.at(3));
    else this->Player4_Last_Action_Label->set_text(" ");
    if(last_actions.size() > 4)
        this->Player5_Last_Action_Label->set_text(last_actions.at(4));
    else this->Player5_Last_Action_Label->set_text(" ");

}

void Gui::update(chat_message message) {
    Mex.lock();

    std::string message_str = std::string(message.body(), message.body_length());
    if(message_str[0] == '{' && this->c->has_gui) {
        json message_json = json::parse(message_str);

        // this->LastPlay_Label->set_text(message_str);

        // std::string names[5];
        std::vector<std::string> names;

        // std::cout << message_json["from"] << std::endl;

        if(message_json["from"] == "server") {

            // std::cout << "getting names from the server" << std::endl;
            names = message_json["names"].get<std::vector<std::string>>();
            // std::cout << "theoretically, the names have been retreived from the server" << std::endl;

            //std::vector<boost::uuids::uuid> uuids = message_json["uuids"].get<std::vector<boost::uuids::uuid>>();
            //gamer->getUUID

            std::vector<std::string> uuids;
            for( auto& uuid : message_json["uuids"]) uuids.push_back(uuid);
            std::string my_uuid = boost::uuids::to_string(this->gamer->getUUID());

            auto it = std::find(uuids.begin(), uuids.end(), my_uuid);
            this->my_index = std::distance(uuids.begin(), it);

            std::rotate(names.begin(), names.begin() + this->my_index, names.end());

            std::vector<int> monies = message_json["balance"].get<std::vector<int>>();
            std::rotate(monies.begin(), monies.begin() + this->my_index, monies.end());

            // this->update_game_info(message_json["pot"], message_json["bet_amount"]);

            if(message_json.find("min bet") != message_json.end()) {
                this->update_slider(message_json["min bet"], monies.at(0));
            }

            if(message_json.find("last actions") != message_json.end()) {
                auto last_actions = message_json["last actions"].get<std::vector<std::string>>();
                std::rotate(last_actions.begin(), last_actions.begin() + this->my_index, last_actions.end());
                this->update_labels(last_actions);
            }

            if(message_json.find("hands") != message_json.end()) {
                std::cout << "Found hand info" << std::endl;
                std::vector<std::string> hands = message_json["hands"].get<std::vector<std::string>>();
                std::rotate(hands.begin(), hands.begin() + this->my_index * 5, hands.end());
                this->set_cards(hands);
                std::cout << "set the hands" << std::endl;
            } else {

                std::vector<std::string> empty_hands;
                this->set_cards(empty_hands);
            }

            if(message_json.find("pot") != message_json.end()
                    && message_json.find("bet_amount") != message_json.end()) {
                this->update_game_info(message_json["pot"], message_json["bet_amount"]);
            }

            bool BetOn = false;
            bool CallOn = false;
            bool FoldOn = false;
            bool SwapOn = false;

            if(message_json["has folded"][this->my_index] == false)
            {
                if(message_json.find("turn") != message_json.end())
                {
                    if(message_json["turn"] == boost::uuids::to_string(this->gamer->getUUID()))
                    {
                        if(message_json.find("possible_actions") != message_json.end())
                        {
                            if(message_json["possible_actions"][0]=="bet")
                            {
                                BetOn = true;
                            }
                            if(message_json["possible_actions"][1]=="call")
                                CallOn = true;

                            if(message_json["possible_actions"][2]=="fold")
                                FoldOn = true;

                            if(message_json["possible_actions"][3]=="swap")
                                SwapOn = true;
                        }
                    }
                }
                else
                {
                    if(message_json.find("possible_actions") != message_json.end())
                    {
                        if(message_json["possible_actions"][0]=="bet")
                            BetOn = true;

                        if(message_json["possible_actions"][1]=="call")
                        {
                            CallOn = true;
                        }
                        if(message_json["possible_actions"][2]=="fold")
                            FoldOn = true;

                        if(message_json["possible_actions"][3]=="swap")
                        {
                            SwapOn = true;
                        }
                    }
                }
            }
            
            if(BetOn && CallOn && FoldOn && !SwapOn)   this->LastPlay_Label->set_text("your turn to Bet");
            if(!BetOn && CallOn && FoldOn && !SwapOn)  this->LastPlay_Label->set_text("your turn to Ante");
            if(!BetOn && !CallOn && !FoldOn && SwapOn) this->LastPlay_Label->set_text("your turn to Swap");

            this->BetButton->set_sensitive(BetOn);
            this->CallButton->set_sensitive(CallOn);
            this->FoldButton->set_sensitive(FoldOn);
            this->SwapButton->set_sensitive(SwapOn);
            std::cout << "button set" << std::endl;

            this->set_names(names);
            this->set_money(monies);
            std::cout << "name and money set" << std::endl;
        }
    }
    Mex.unlock();
}
