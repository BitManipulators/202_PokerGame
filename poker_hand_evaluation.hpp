#pragma once

#include <vector>
#include <string>

// Hand categories (the higher, the better).
enum PokerHandEvaluationCategory {
    Unknown = 0,
    HighCard = 1,
    OnePair,
    TwoPair,
    ThreeOfAKind,
    Straight,
    Flush,
    FullHouse,
    FourOfAKind,
    StraightFlush,
    RoyalFlush,
};

struct PokerHandEvaluation {
    PokerHandEvaluationCategory category;
    // A vector of tiebreaker values in descending order.
    std::vector<int> tiebreakers;

    // Compare two evaluations.
    bool operator>(const PokerHandEvaluation &other) const;

    // Convert the PokerHandEvaluationCategory to a string representation.
    std::string to_string() const;
};

const PokerHandEvaluation lowest_evaluation = {HighCard, {0}};
