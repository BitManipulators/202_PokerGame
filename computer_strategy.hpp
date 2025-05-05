#pragma once

#include "move.hpp"
#include "card.hpp"
#include "game_constants.hpp"
#include "game_state.hpp"

#include <vector>

class ComputerStrategy {
    public :
        virtual ~ComputerStrategy() = default;
        virtual Move get_next_move(GameState current_state) = 0;

};

class EasyStrategy : public ComputerStrategy {
    Move get_next_move(GameState current_state) override;

};

class MediumStrategy : public ComputerStrategy {
    Move get_next_move(GameState current_state) override;

private:
    bool is_suited(const std::vector<const Card*>& hand);
    int evaluate_preflop(const std::vector<const Card*>& hand);
    int evaluate_hand_strength(const std::vector<const Card*>& hand,
                         const std::vector<const Card*>& community,
                         PokerEngineEnumState stage);
};

