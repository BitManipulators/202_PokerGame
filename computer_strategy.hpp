#pragma once
#include "move.hpp"
#include "card.hpp"
#include "game_constants.hpp"
#include "game_state.hpp"
#include <vector>
#include <set>



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
    std::map<Rank, int> getRankCounts(const std::vector<const Card*>& cards);
    int evaluatePreflop(const std::vector<const Card*>& hand);
    int evaluateHandStrength(const std::vector<const Card*>& hand,
                         const std::vector<const Card*>& community,
                         PokerEngineEnumState stage);


};

