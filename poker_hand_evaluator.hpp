#pragma once

#include "pokerhand.hpp"

enum class PokerHandWinner {
    Player1,
    Player2,
    Tie
};

struct PokerHandResult {
    PokerHandWinner winner;
    PokerHandEvaluation evaluation;
};

class PokerHandEvaluator {
public:
    static PokerHandEvaluation evaluate_hand(const std::vector<const Card*>& player_cards,
                                             const std::vector<const Card*>& community_cards);

    static PokerHandResult determine_winner(const std::vector<const Card*>& player1_cards,
                                            const std::vector<const Card*>& player2_cards,
                                            const std::vector<const Card*>& community_cards);
};
