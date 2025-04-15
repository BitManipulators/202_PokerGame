#pragma once
#include "poker_game_state.hpp"

class FlopState : public PokerGameState {
public:
    void handle(PokerGame& game) override;
    const char* name() const override { return "Flop"; }
    QString getMessage() const override {
        return "Flop has been dealt.";
    }
};
