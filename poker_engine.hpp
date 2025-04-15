#pragma once

#include "move.hpp"
#include "poker_engine_state.hpp"
#include "poker_game.hpp"

class PokerEngineTests;

class PokerEngine {
public:
    PokerEngine();
    ~PokerEngine();

    GameAction::Result new_game();
    GameAction::Result make_move(PlayerType player_type, Move move);

    const PokerGame& get_game();

    friend class PokerEngineTests;

private:
    PokerGame* game;
    PokerEngineState* state;
    PokerEngineStatesBlock* block;
};
