#pragma once

#include "move.hpp"
#include "poker_engine_state.hpp"
#include "poker_game.hpp"

class PokerEngineTests;

enum class ResultState{
    HUMAN_PLAYER_ACTION,
    HUMAN_PLAYER_ACTION_ERROR,
    GAME_ENDED
};


struct ControllerResult{
    ResultState state;
    std::optional<Move> move;
    std::optional<std::string> error_message;
};

class PokerEngine {
public:
    PokerEngine(PokerGame &poker_game);
    ~PokerEngine();

    GameAction::Result new_game();
    GameAction::Result make_move(PlayerType player_type, Move move);
    ControllerResult make_moves();

    friend class PokerEngineTests;

private:
    PokerGame& game;
    PokerEngineState* state;
    PokerEngineStatesBlock* block;
};
