#pragma once

#include "card.hpp"

#include <vector>

class Deck {
public:
    Deck();
    void shuffle();
    const Card* deal_card();
    bool is_empty() const;
private:
    std::vector<std::shared_ptr<const Card>> cards;
};
