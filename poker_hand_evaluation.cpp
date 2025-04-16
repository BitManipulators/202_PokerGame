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

std::string PokerHandEvaluation::to_string() const {
    std::string hand_type;
    switch (category) {
            case PokerHandEvaluationCategory::HighCard:
                hand_type = "High Card";
                break;
            case PokerHandEvaluationCategory::OnePair:
                hand_type = "One Pair";
                break;
            case PokerHandEvaluationCategory::TwoPair:
                hand_type = "Two Pair";
                break;
            case PokerHandEvaluationCategory::ThreeOfAKind:
                hand_type = "Three of a Kind";
                break;
            case PokerHandEvaluationCategory::Straight:
                hand_type = "Straight";
                break;
            case PokerHandEvaluationCategory::Flush:
                hand_type = "Flush";
                break;
            case PokerHandEvaluationCategory::FullHouse:
                hand_type = "Full House";
                break;
            case PokerHandEvaluationCategory::FourOfAKind:
                hand_type = "Four of a Kind";
                break;
            case PokerHandEvaluationCategory::StraightFlush:
                // Check if it's a royal flush by examining the tiebreakers
                if (!tiebreakers.empty() && tiebreakers[0] == 14) { // Ace high
                    hand_type = "Royal Flush";
                } else {
                    hand_type = "Straight Flush";
                }
                break;
            default:
                hand_type = "Unknown hand";
                break;
        }
    return hand_type;
}