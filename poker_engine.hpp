#pragma once

#include "poker_engine_state.hpp"
#include "poker_game.hpp"

class PokerEngineTests;

class PokerEngine {
public:
    PokerEngine();
    ~PokerEngine();

    GameAction::Result new_game();
    GameAction::Result call(PlayerType player_type);
    GameAction::Result fold(PlayerType player_type);
    GameAction::Result raise(PlayerType player_type, const std::size_t value);

    const PokerGame& get_game();

    friend class PokerEngineTests;

private:
    PokerGame* game;
    PokerEngineState* state;
    PokerEngineStatesBlock* block;
};
