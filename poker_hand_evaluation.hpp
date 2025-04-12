#pragma once

#include <vector>

// Hand categories (the higher, the better).
enum PokerHandEvaluationCategory {
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

struct PokerHandEvaluation {
    PokerHandEvaluationCategory category;
    // A vector of tiebreaker values in descending order.
    std::vector<int> tiebreakers;

    // Compare two evaluations.
    bool operator>(const PokerHandEvaluation &other) const;
};

const PokerHandEvaluation lowest_evaluation = {HighCard, {0}};
