#pragma once
#include "move.hpp"
#include "card.hpp"
#include "game_constants.hpp"
#include <vector>


class ComputerStrategy {

    public :
        virtual ~ ComputerStrategy() = default;
        virtual Move getNextMove() = 0;

};

class EasyStrategy : public ComputerStrategy {

    Move getNextMove() override;

};

class MediumStrategy : public ComputerStrategy {
    Move getNextMove() override;
};

