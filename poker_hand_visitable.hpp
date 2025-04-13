#pragma once

#include "poker_hand_visitor.hpp"

class PokerHandVisitable {
public:
    virtual ~PokerHandVisitable() = default;
    virtual const PokerHandEvaluation accept(const PokerHandVisitor& poker_hand_visitor) const = 0;
};
