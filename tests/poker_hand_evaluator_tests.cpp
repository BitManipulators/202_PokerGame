#include "../poker_hand_evaluator.hpp"

#include <gtest/gtest.h>

class PokerHandEvaluatorTests : public ::testing::Test {

};

TEST(PokerHandEvaluatorTests, RoyalFlush) {
    std::vector<const Card*> player1_hole_cards {
        Card::getCard(Suit::Clubs, Rank::Ace).get(),
        Card::getCard(Suit::Clubs, Rank::King).get(),
    };

    std::vector<const Card*> player2_hole_cards {
        Card::getCard(Suit::Hearts, Rank::Five).get(),
        Card::getCard(Suit::Hearts, Rank::Four).get(),
    };

    std::vector<const Card*> community_cards {
        Card::getCard(Suit::Clubs, Rank::Queen).get(),
        Card::getCard(Suit::Clubs, Rank::Jack).get(),
        Card::getCard(Suit::Clubs, Rank::Ten).get(),
        Card::getCard(Suit::Clubs, Rank::Nine).get(),
        Card::getCard(Suit::Clubs, Rank::Eight).get(),
    };

    PokerHandResult poker_hand_result = PokerHandEvaluator::determine_winner(
        player1_hole_cards, player2_hole_cards, community_cards);

    EXPECT_EQ(poker_hand_result.winner, PokerHandWinner::Player1);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
