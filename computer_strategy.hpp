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
        virtual ~ ComputerStrategy() = default;
        virtual Move getNextMove(GameState current_state) = 0;

};

class EasyStrategy : public ComputerStrategy {
    Move getNextMove(GameState current_state) override;

};

class MediumStrategy : public ComputerStrategy {
    Move getNextMove(GameState current_state) override;

private:
    bool isSuited(const std::vector<const Card*>& hand);
    int evaluatePreflop(const std::vector<const Card*>& hand);
    int evaluateHandStrength(const std::vector<const Card*>& hand,
                         const std::vector<const Card*>& community,
                         PokerEngineEnumState stage);
    
    std::size_t calculatePotPortion(int strength, std::size_t pot); 
    Move handleLowChipCount(int strength);    
    Move handleNormalBetting(int strength, std::size_t raise_amount, std::size_t current_bet);          
    int getHandCategoryScore(PokerHandEvaluationCategory category);
    
    const int MINIMUM_BET_MULTIPLIER = 2;
    const int FOLD_CHANCE_LOW_HAND = 30;
    const int FOLD_CHANCE_LOW_HAND_LOW_BET = 50;
    const int CALL_CHANCE_WEAKER_HAND = 60;
    const int HIGH_HAND_THRESHOLD = 80;
    const int MEDIUM_HAND_THRESHOLD = 60;
    const int WEAK_HAND_THRESHOLD = 40;
    const int STRONG_PAIR_THRESHOLD = 10;
    const int STRONG_SUITED_THRESHOLD = 9;                      


};

class HardStrategy : public ComputerStrategy {
    Move getNextMove(GameState current_state) override;
};    

