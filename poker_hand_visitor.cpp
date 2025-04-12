#include "poker_hand_visitor.hpp"

#include "pokerhand.hpp"
#include "card.hpp"

#include <algorithm>

namespace {

// Checks if hand is flush (all cards same suit).
bool isFlush(const PokerHand* hand) {
    const std::vector<const Card*>& cards = hand->get_cards();

    Suit firstSuit = cards[0]->getSuit();
    for (size_t i = 1; i < cards.size(); i++) {
        if (cards[i]->getSuit() != firstSuit)
            return false;
    }

    return true;
}

// Checks if hand is straight.
// Returns highest card value of the straight, or 0 if not a straight.
int isStraight(const PokerHand* hand) {
    std::vector<int> values;
    for (const Card* c : hand->get_cards()) {
        values.push_back(c->getValue());
    }

    // Remove duplicates.
    std::sort(values.begin(), values.end());
    values.erase(std::unique(values.begin(), values.end()), values.end());

    // Check for Ace low straight.
    if (values.size() >= 5) {
        // Check normal straight.
        for (size_t i = 0; i <= values.size() - 5; i++) {
            bool straight = true;
            for (size_t j = 1; j < 5; j++) {
                if (values[i + j] != values[i] + static_cast<int>(j)) {
                    straight = false;
                    break;
                }
            }
            if (straight)
                return values[i + 4]; // highest card in the straight.
        }
        // Special case: Ace, 2, 3, 4, 5.
        // If Ace exists (14) and 2,3,4,5 exist.
        if (std::find(values.begin(), values.end(), 14) != values.end() &&
            std::find(values.begin(), values.end(), 2) != values.end() &&
            std::find(values.begin(), values.end(), 3) != values.end() &&
            std::find(values.begin(), values.end(), 4) != values.end() &&
            std::find(values.begin(), values.end(), 5) != values.end()) {
            return 5;
        }
    }
    return 0;
}

} // namespace

const PokerHandEvaluation StraightFlushVisitor::visit(const PokerHand* poker_hand) const {
    bool flush = isFlush(poker_hand);
    int straightHigh = isStraight(poker_hand);

    // Check Straight Flush.
    if (flush && straightHigh) {
        return {StraightFlush, {straightHigh}};
    }

    return lowest_evaluation;
}

FourOfAKindVisitor::FourOfAKindVisitor(const std::vector<std::pair<int,int>>& card_frequencies)
    : card_frequencies_(card_frequencies) {}

const PokerHandEvaluation FourOfAKindVisitor::visit(const PokerHand* poker_hand) const {
    // Check Four of a Kind.
    if (card_frequencies_[0].first == 4) {
        // kicker is the highest remaining card.
        int kicker = 0;
        for (const auto &p : card_frequencies_) {
            if (p.first != 4) {
                kicker = p.second;
                break;
            }
        }

        return {FourOfAKind, {card_frequencies_[0].second, kicker}};
    }

    return lowest_evaluation;
}

FullHouseVisitor::FullHouseVisitor(const std::vector<std::pair<int,int>>& card_frequencies)
    : card_frequencies_(card_frequencies) {}

const PokerHandEvaluation FullHouseVisitor::visit(const PokerHand* poker_hand) const {
    // Check Full House.
    if (card_frequencies_[0].first == 3 && card_frequencies_.size() > 1 && card_frequencies_[1].first >= 2) {
        return {FullHouse, {card_frequencies_[0].second, card_frequencies_[1].second}};
    }

    return lowest_evaluation;
}

const PokerHandEvaluation FlushVisitor::visit(const PokerHand* poker_hand) const {
    // Check Flush.
    if (isFlush(poker_hand)) {
        std::vector<int> tiebreakers;
        for (const Card* c : poker_hand->get_cards()) {
            tiebreakers.push_back(c->getValue());
        }
        return {Flush, tiebreakers};
    }

    return lowest_evaluation;
}

const PokerHandEvaluation StraightVisitor::visit(const PokerHand* poker_hand) const {
    // Check Straight.
    int straightHigh = isStraight(poker_hand);

    if (straightHigh) {
        return {Straight, {straightHigh}};
    }

    return lowest_evaluation;
}

ThreeOfAKindVisitor::ThreeOfAKindVisitor(const std::vector<std::pair<int,int>>& card_frequencies)
    : card_frequencies_(card_frequencies) {}

const PokerHandEvaluation ThreeOfAKindVisitor::visit(const PokerHand* poker_hand) const {
    // Check Three of a Kind.
    if (card_frequencies_[0].first == 3) {
        std::vector<int> tiebreakers = {card_frequencies_[0].second};
        // Add remaining cards.
        for (const auto &p : card_frequencies_) {
            if (p.first != 3) tiebreakers.push_back(p.second);
        }
        return {ThreeOfAKind, tiebreakers};
    }

    return lowest_evaluation;
}

TwoPairVisitor::TwoPairVisitor(const std::vector<std::pair<int,int>>& card_frequencies)
    : card_frequencies_(card_frequencies) {}

const PokerHandEvaluation TwoPairVisitor::visit(const PokerHand* poker_hand) const {
    // Check Two Pair.
    if (card_frequencies_[0].first == 2 && card_frequencies_.size() > 1 && card_frequencies_[1].first == 2) {
        int highPair = card_frequencies_[0].second;
        int lowPair = card_frequencies_[1].second;
        int kicker = 0;
        for (const auto &p : card_frequencies_) {
            if (p.first == 1) {
                kicker = p.second;
                break;
            }
        }
        return {TwoPair, {highPair, lowPair, kicker}};
    }

    return lowest_evaluation;
}

OnePairVisitor::OnePairVisitor(const std::vector<std::pair<int,int>>& card_frequencies)
    : card_frequencies_(card_frequencies) {}

const PokerHandEvaluation OnePairVisitor::visit(const PokerHand* poker_hand) const {
    // Check One Pair.
    if (card_frequencies_[0].first == 2) {
        std::vector<int> tiebreakers = {card_frequencies_[0].second};
        for (const auto &p : card_frequencies_) {
            if (p.first == 1) tiebreakers.push_back(p.second);
        }
        return {OnePair, tiebreakers};
    }

    return lowest_evaluation;
}

const PokerHandEvaluation HighCardVisitor::visit(const PokerHand* poker_hand) const {
    // Check High Card.
    {
        std::vector<int> tiebreakers;
        for (const Card* c : poker_hand->get_cards()) {
            tiebreakers.push_back(c->getValue());
        }
        return {HighCard, tiebreakers};
    }

    return lowest_evaluation;
}
