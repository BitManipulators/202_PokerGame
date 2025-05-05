#pragma once

#include "card.hpp"

#include "poker_hand.hpp"
#include "poker_hand_evaluation.hpp"

#include <optional>

enum class PokerHandWinner {
    Player1,
    Player2,
    Tie
};

struct PokerHandResult {
    std::optional<PokerHand> hand;
    PokerHandWinner winner;
    PokerHandEvaluation evaluation;
};

class PokerHandEvaluator {
public:
    static std::tuple<PokerHand, PokerHandEvaluation> evaluate_hand(const std::vector<const Card*>& player_cards,
                                                                    const std::vector<const Card*>& community_cards);

    static PokerHandResult determine_winner(const std::vector<const Card*>& player1_cards,
                                            const std::vector<const Card*>& player2_cards,
                                            const std::vector<const Card*>& community_cards);
};
