#pragma once
#include "poker_game_state.hpp"

class RiverState : public PokerGameState {
public:
    void handle(PokerGame& game) override;
    const char* name() const override { return "River"; }
    QString getMessage() const override {
        return "River has been dealt.";
    }
};
