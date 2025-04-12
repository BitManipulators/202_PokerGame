#include "pokerhand.hpp"

PokerHand::PokerHand(std::vector<const Card*>& cards)
    : cards_(cards) {}

const std::vector<const Card*>& PokerHand::get_cards() const {
    return cards_;
}

const PokerHandEvaluation PokerHand::accept(const PokerHandVisitor& visitor) const {
    return visitor.visit(this);
}
