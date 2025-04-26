#pragma once

#include "move.hpp"
#include "poker_game.hpp"
#include "poker_engine.hpp"
#include "console_logger.hpp"

#include <string>
#include <optional>

enum class UIState{
    HUMAN_PLAYER_ACTION,
    HUMAN_PLAYER_ACTION_ERROR,
    HUMAN_PLAYER_ACTION_UNKNOWN,
    GAME_ENDED
};

struct UIComponents
{
    
    std::vector<const Card *> community_cards;
    std::vector<const Card *> human_player_cards;
    std::vector<const Card *> computer_player_cards;
    size_t current_bet;
    size_t pot_value;
    std::optional<Move> computer_move;
    std::optional<std::string> error_message;
    std::size_t human_chips;
    std::size_t computer_chips;
    std::optional<PokerHandWinner> winner;
    bool is_game_ended;
    std::string winning_hand_desc;
};

struct GameControllerResult
{
    UIState state;
    UIComponents ui_components;
};

class GameController
{

public:
    GameController() : game(), engine(game) {
        
        //game.add_observer(this);
        //ConsoleLogger* consoleLogger = new ConsoleLogger();
        //game.add_observer(consoleLogger);
    }

    UIState new_game();
    UIState next_move();
    void set_human_made_move();
    void set_human_move(Move move);
    UIComponents get_ui_components() const;

private:
    
    PokerGame game;
    PokerEngine engine;
    bool human_player_made_ui_choice = false;
    //UIComponents get_ui_components() const;
    Move get_computer_move() const;
    Move get_human_move() const;
   

};
