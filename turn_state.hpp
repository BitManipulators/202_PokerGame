#pragma once
#include "poker_game_state.hpp"

class TurnState : public PokerGameState {
public:
    void handle(PokerGame& game) override;
    const char* name() const override { return "Turn"; }
    QString getMessage() const override {
        return "Turn has been dealt.";
    }
};
