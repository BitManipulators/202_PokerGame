#include "poker_hand_evaluator.hpp"

#include "poker_hand.hpp"

#include <algorithm>
#include <memory>

namespace {

// Compare function for sorting cards in descending order.
bool compare_card_desc(const Card* a, const Card* b) {
    return a->get_value() > b->get_value();
}

// Create all possible 5 card combinations from 7 cards (2 player cards and 5 community cards)
std::vector<PokerHand> all_five_card_combinations(const std::vector<const Card*>& player_cards,
                                                  const std::vector<const Card*>& community_cards) {
    std::vector<const Card*> all_cards{};
    all_cards.insert(all_cards.begin(), player_cards.begin(), player_cards.end());
    all_cards.insert(all_cards.begin(), community_cards.begin(), community_cards.end());

    // Use indices for combination: choose 5 out of n.
    std::vector<int> indices(all_cards.size(), 0);
    // First 5 indices set to 1.
    std::fill(indices.begin(), indices.begin() + POKER_HAND_SIZE, 1);
    std::sort(indices.begin(), indices.end(), std::greater<int>());

    std::vector<PokerHand> poker_hands{};
    do {
        std::vector<const Card*> combination;
        for (int i = 0; i < all_cards.size(); i++) {
            if (indices[i] == 1) {
                combination.push_back(all_cards[i]);
            }
        }

        // Sort cards by rank
        std::sort(combination.begin(), combination.end(), compare_card_desc);

        poker_hands.push_back(PokerHand(combination));
    } while (std::prev_permutation(indices.begin(), indices.end()));

    return poker_hands;
}

// Create the visitors
std::vector<std::unique_ptr<PokerHandVisitor>> make_visitors(const std::vector<std::pair<int,int>>& card_frequencies) {
    std::vector<std::unique_ptr<PokerHandVisitor>> visitors;

    visitors.emplace_back(std::make_unique<StraightFlushVisitor>());
    visitors.emplace_back(std::make_unique<FourOfAKindVisitor>(card_frequencies));
    visitors.emplace_back(std::make_unique<FullHouseVisitor>(card_frequencies));
    visitors.emplace_back(std::make_unique<FlushVisitor>());
    visitors.emplace_back(std::make_unique<StraightVisitor>());
    visitors.emplace_back(std::make_unique<ThreeOfAKindVisitor>(card_frequencies));
    visitors.emplace_back(std::make_unique<TwoPairVisitor>(card_frequencies));
    visitors.emplace_back(std::make_unique<OnePairVisitor>(card_frequencies));
    visitors.emplace_back(std::make_unique<HighCardVisitor>());

    return visitors;
}

std::tuple<PokerHand, PokerHandEvaluation> evaluate_poker_hands(const std::vector<PokerHand>& poker_hands) {
    PokerHand best_hand = poker_hands.at(0);
    PokerHandEvaluation best_evaluation = lowest_evaluation;

    for (const PokerHand& poker_hand : poker_hands) {
        // Count rank frequencies.
        std::map<int, int> freq;
        for (const Card* c : poker_hand.get_cards()) {
            freq[c->get_value()]++;
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

        std::vector<std::unique_ptr<PokerHandVisitor>> visitors = make_visitors(freqVec);

        // Begin hand evaluation using visitors
        PokerHandEvaluation visitor_evaluation = lowest_evaluation;
        for (std::unique_ptr<PokerHandVisitor>& visitor : visitors) {
            PokerHandEvaluation visitor_evaluation = poker_hand.accept(*visitor);
            if (visitor_evaluation > best_evaluation) {
                best_hand = poker_hand;
                best_evaluation = visitor_evaluation;
                break;
            }
        }
    }

    return std::tuple(best_hand, best_evaluation);
}

} // namespace

std::tuple<PokerHand, PokerHandEvaluation> PokerHandEvaluator::evaluate_hand(const std::vector<const Card*>& player_cards,
                                                      const std::vector<const Card*>& community_cards) {

    // Ensure we have enough cards (this assumes at least 3 community cards are dealt)
    if (player_cards.size() != 2 || community_cards.size() < 3) {
        throw std::runtime_error("Not enough cards to evaluate.");
    }

    std::vector<PokerHand> player_poker_hands = all_five_card_combinations(player_cards, community_cards);

    auto [player_best_hand, player_best_evaluation] = evaluate_poker_hands(player_poker_hands);

    return std::tuple(player_best_hand, player_best_evaluation);
}

PokerHandResult PokerHandEvaluator::determine_winner(const std::vector<const Card*>& player1_cards,
                                                     const std::vector<const Card*>& player2_cards,
                                                     const std::vector<const Card*>& community_cards) {

    // Ensure we have enough cards (this assumes all 5 community cards are dealt)
    if (player1_cards.size() != 2 || player2_cards.size() != 2 || community_cards.size() != 5) {
        throw std::runtime_error("Not enough cards to evaluate.");
    }

    auto [player1_best_hand, player1_best_evaluation] = evaluate_hand(player1_cards, community_cards);
    auto [player2_best_hand, player2_best_evaluation] = evaluate_hand(player2_cards, community_cards);

    if (player1_best_evaluation > player2_best_evaluation) {
        return {player1_best_hand, PokerHandWinner::Player1, player1_best_evaluation};
    } else if (player2_best_evaluation > player1_best_evaluation) {
        return {player2_best_hand, PokerHandWinner::Player2, player2_best_evaluation};
    } else {
        return {std::nullopt, PokerHandWinner::Tie, player1_best_evaluation};
    }
}
