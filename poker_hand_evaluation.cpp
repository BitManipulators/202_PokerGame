#include "poker_hand_evaluation.hpp"

// Comparison operator for HandEvaluation.
// First compare category, then each tiebreaker in order.
bool PokerHandEvaluation::operator>(const PokerHandEvaluation &other) const {
    if (category != other.category)
        return category > other.category;
    // Compare tiebreakers.
    size_t n = std::min(tiebreakers.size(), other.tiebreakers.size());
    for (size_t i = 0; i < n; i++) {
        if (tiebreakers[i] != other.tiebreakers[i])
            return tiebreakers[i] > other.tiebreakers[i];
    }
    // They are equal.
    return false;
}
