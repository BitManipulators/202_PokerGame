#ifndef POKERHAND_H
#define POKERHAND_H

#include <vector>
#include "card.hpp"

// Hand categories (the higher, the better).
enum HandCategory {
    HighCard = 1,
    OnePair,
    TwoPair,
    ThreeOfAKind,
    Straight,
    Flush,
    FullHouse,
    FourOfAKind,
    StraightFlush
};

struct HandEvaluation {
    HandCategory category;
    // A vector of tiebreaker values in descending order.
    std::vector<int> tiebreakers;

    // Compare two evaluations.
    bool operator>(const HandEvaluation &other) const;
};

HandEvaluation evaluateFiveCardHand(const std::vector<Card>& hand);
HandEvaluation bestHandFromSeven(const std::vector<Card>& sevenCards);

#endif // POKERHAND_H
