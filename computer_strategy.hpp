#pragma once

#include "move.hpp"
#include "card.hpp"
#include "game_constants.hpp"
#include "game_state.hpp"

#include <vector>
#include <set>
#include "poker_hand_evaluation.hpp"



class ComputerStrategy {
    public :
        virtual ~ComputerStrategy() = default;
        virtual Move get_next_move(GameState current_state) = 0;
        bool can_raise(GameState current_state);
        const int MINIMUM_BET_MULTIPLIER = 2;
        

};

class EasyStrategy : public ComputerStrategy {
    Move get_next_move(GameState current_state) override;
    Move handle_low_chip_count(int strength);

};

class MediumStrategy : public ComputerStrategy {
    Move get_next_move(GameState current_state) override;

private:
    bool is_suited(const std::vector<const Card*>& hand);
    int evaluate_preflop(const std::vector<const Card*>& hand);
    int evaluate_hand_strength(const std::vector<const Card*>& hand,
                         const std::vector<const Card*>& community,
                         PokerEngineEnumState stage);
    
    std::size_t calculate_pot_portion(int strength, std::size_t pot); 
    Move handle_low_chip_count(int strength);    
    Move handle_normal_betting(int strength, std::size_t raise_amount, std::size_t current_bet);          
    int get_hand_category_score(PokerHandEvaluationCategory category);
    
   
    const int FOLD_CHANCE_LOW_HAND = 30;
    const int FOLD_CHANCE_LOW_HAND_LOW_BET = 50;
    const int CALL_CHANCE_WEAKER_HAND = 60;
    const int HIGH_HAND_THRESHOLD = 80;
    const int MEDIUM_HAND_THRESHOLD = 60;
    const int WEAK_HAND_THRESHOLD = 40;
    const int STRONG_PAIR_THRESHOLD = 10;
    const int STRONG_SUITED_THRESHOLD = 9;                      


};

// class HardStrategy : public ComputerStrategy {
//     Move get_next_move(GameState current_state) override;
    
// };    

