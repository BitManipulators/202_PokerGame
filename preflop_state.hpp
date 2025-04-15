#pragma once
#include "poker_game_state.hpp"

class PreFlopState : public PokerGameState {
public:
    void handle(PokerGame& game) override;
    const char* name() const override { return "PreFlop"; }
};
