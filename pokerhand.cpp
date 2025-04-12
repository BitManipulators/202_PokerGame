#include "pokerhand.hpp"
#include "pokergame.hpp"
#include <algorithm>
#include <map>
#include <stdexcept>

// Helper: convert card rank (1-13) to a numeric value with Ace high (14).
int cardValue(const Card &card) {
    Rank rank = card.getRank();
    return (int)rank;
}

// Compare function for sorting cards in descending order.
bool compareCardDesc(const Card* a, const Card* b) {
    return cardValue(*a) > cardValue(*b);
}

// Checks if hand is flush (all cards same suit).
bool isFlush(const std::vector<const Card*>& hand) {
    Suit firstSuit = hand[0]->getSuit();
    for (size_t i = 1; i < hand.size(); i++) {
        if (hand[i]->getSuit() != firstSuit)
            return false;
    }
    return true;
}

// Checks if hand is straight.
// Returns highest card value of the straight, or 0 if not a straight.
int isStraight(const std::vector<const Card*>& hand) {
    std::vector<int> values;
    for (const Card* c : hand)
        values.push_back(cardValue(*c));

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

int PokerGame::determineWinner() {
    // Combine each player's hole cards with the community cards.
    std::vector<const Card*> player1Cards = player1.getHand();
    std::vector<const Card*> player2Cards = player2.getHand();

    // Add community cards.
    for (const Card* c : communityCards) {
        player1Cards.push_back(c);
        player2Cards.push_back(c);
    }

    // Ensure we have enough cards (this assumes all 5 community cards are dealt)
    if (player1Cards.size() < 7 || player2Cards.size() < 7) {
        throw std::runtime_error("Not enough cards to evaluate the best hand.");
    }

    HandEvaluation eval1 = bestHandFromSeven(player1Cards);
    HandEvaluation eval2 = bestHandFromSeven(player2Cards);

    if (eval1 > eval2)
        return 1;
    else if (eval2 > eval1)
        return 2;
    else
        return 0; // tie
}



// Evaluate a 5-card hand and return its evaluation.
HandEvaluation evaluateFiveCardHand(const std::vector<const Card*>& hand) {
    // Make a copy and sort descending.
    std::vector<const Card*> sortedHand = hand;
    std::sort(sortedHand.begin(), sortedHand.end(), compareCardDesc);

    // Count rank frequencies.
    std::map<int, int> freq;
    for (const Card* c : sortedHand) {
        freq[cardValue(*c)]++;
    }

    // Create a sorted vector of pairs (frequency, rank) for tiebreakers.
    // We sort first by frequency descending, then by rank descending.
    std::vector<std::pair<int,int>> freqVec;
    for (auto &p : freq) {
        freqVec.push_back({p.second, p.first});
    }
    std::sort(freqVec.begin(), freqVec.end(), [](const std::pair<int,int>& a, const std::pair<int,int>& b) {
        if (a.first == b.first)
            return a.second > b.second;
        return a.first > b.first;
    });

    bool flush = isFlush(sortedHand);
    int straightHigh = isStraight(sortedHand);

    // Check for Straight Flush.
    if (flush && straightHigh) {
        return {StraightFlush, {straightHigh}};
    }

    // Check Four of a Kind.
    if (freqVec[0].first == 4) {
        // kicker is the highest remaining card.
        int kicker = 0;
        for (const auto &p : freqVec) {
            if (p.first != 4) {
                kicker = p.second;
                break;
            }
        }
        return {FourOfAKind, {freqVec[0].second, kicker}};
    }

    // Check Full House.
    if (freqVec[0].first == 3 && freqVec.size() > 1 && freqVec[1].first >= 2) {
        return {FullHouse, {freqVec[0].second, freqVec[1].second}};
    }

    // Flush.
    if (flush) {
        std::vector<int> tiebreakers;
        for (const Card* c : sortedHand) {
            tiebreakers.push_back(cardValue(*c));
        }
        return {Flush, tiebreakers};
    }

    // Straight.
    if (straightHigh) {
        return {Straight, {straightHigh}};
    }

    // Three of a Kind.
    if (freqVec[0].first == 3) {
        std::vector<int> tiebreakers = {freqVec[0].second};
        // Add remaining cards.
        for (const auto &p : freqVec) {
            if (p.first != 3) tiebreakers.push_back(p.second);
        }
        return {ThreeOfAKind, tiebreakers};
    }

    // Two Pair.
    if (freqVec[0].first == 2 && freqVec.size() > 1 && freqVec[1].first == 2) {
        int highPair = freqVec[0].second;
        int lowPair = freqVec[1].second;
        int kicker = 0;
        for (const auto &p : freqVec) {
            if (p.first == 1) {
                kicker = p.second;
                break;
            }
        }
        return {TwoPair, {highPair, lowPair, kicker}};
    }

    // One Pair.
    if (freqVec[0].first == 2) {
        std::vector<int> tiebreakers = {freqVec[0].second};
        for (const auto &p : freqVec) {
            if (p.first == 1) tiebreakers.push_back(p.second);
        }
        return {OnePair, tiebreakers};
    }

    // High Card.
    {
        std::vector<int> tiebreakers;
        for (const Card* c : sortedHand) {
            tiebreakers.push_back(cardValue(*c));
        }
        return {HighCard, tiebreakers};
    }
}

// Comparison operator for HandEvaluation.
// First compare category, then each tiebreaker in order.
bool HandEvaluation::operator>(const HandEvaluation &other) const {
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

// Generate all 5-card combinations from 7 cards and return the best evaluation.
HandEvaluation bestHandFromSeven(const std::vector<const Card*>& sevenCards) {
    int n = sevenCards.size();
    if (n < 5)
        throw std::runtime_error("Need at least 5 cards to evaluate a hand.");

    HandEvaluation bestEval = {HighCard, {0}};

    // Use indices for combination: choose 5 out of n.
    std::vector<int> indices(n, 0);
    // First 5 indices set to 1.
    std::fill(indices.begin(), indices.begin() + 5, 1);
    std::sort(indices.begin(), indices.end(), std::greater<int>());

    bool first = true;
    do {
        std::vector<const Card*> combo;
        for (int i = 0; i < n; i++) {
            if (indices[i] == 1) {
                combo.push_back(sevenCards[i]);
            }
        }
        HandEvaluation currentEval = evaluateFiveCardHand(combo);
        if (first || currentEval > bestEval) {
            bestEval = currentEval;
            first = false;
        }
    } while (std::prev_permutation(indices.begin(), indices.end()));

    return bestEval;
}
