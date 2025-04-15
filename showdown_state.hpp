#pragma once
#include "poker_game_state.hpp"

// Final state of the game
class ShowDownState : public PokerGameState {
public:
    void handle(PokerGame& game) override;
    const char* name() const override { return "Showdown"; }
    QString getMessage() const override {
        return "All betting rounds complete. Ready for showdown!! Click Determine Winner.";
    }
};
