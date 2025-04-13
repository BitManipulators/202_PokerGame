#pragma once

#include "card.hpp"
#include "poker_hand_visitable.hpp"

#include <vector>

constexpr std::size_t POKER_HAND_SIZE = 5;

class PokerHand : public PokerHandVisitable {
public:
    PokerHand(std::vector<const Card*>& cards);

    const PokerHandEvaluation accept(const PokerHandVisitor& visitor) const override;
    const std::vector<const Card*>& get_cards() const;

private:
    std::vector<const Card*> cards_;
};
