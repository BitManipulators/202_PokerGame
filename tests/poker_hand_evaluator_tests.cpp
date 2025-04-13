#include "../poker_hand_evaluator.hpp"

#include <gtest/gtest.h>

class PokerHandEvaluatorTests : public ::testing::Test {};

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
    EXPECT_EQ(poker_hand_result.evaluation.category, PokerHandEvaluationCategory::StraightFlush);
    EXPECT_EQ(poker_hand_result.evaluation.tiebreakers[0], (int)Rank::Ace);
}

TEST(PokerHandEvaluatorTests, FourOfAKind) {
    std::vector<const Card*> player1_hole_cards {
        Card::getCard(Suit::Clubs, Rank::Ace).get(),
        Card::getCard(Suit::Hearts, Rank::Ace).get(),
    };

    std::vector<const Card*> player2_hole_cards {
        Card::getCard(Suit::Hearts, Rank::Five).get(),
        Card::getCard(Suit::Hearts, Rank::Four).get(),
    };

    std::vector<const Card*> community_cards {
        Card::getCard(Suit::Clubs, Rank::Queen).get(),
        Card::getCard(Suit::Clubs, Rank::Jack).get(),
        Card::getCard(Suit::Spades, Rank::Ace).get(),
        Card::getCard(Suit::Clubs, Rank::Nine).get(),
        Card::getCard(Suit::Diamonds, Rank::Ace).get(),
    };

    PokerHandResult poker_hand_result = PokerHandEvaluator::determine_winner(
        player1_hole_cards, player2_hole_cards, community_cards);

    EXPECT_EQ(poker_hand_result.winner, PokerHandWinner::Player1);
    EXPECT_EQ(poker_hand_result.evaluation.category, PokerHandEvaluationCategory::FourOfAKind);
    EXPECT_EQ(poker_hand_result.evaluation.tiebreakers[0], (int)Rank::Ace);
    EXPECT_EQ(poker_hand_result.evaluation.tiebreakers[1], (int)Rank::Queen);
}

TEST(PokerHandEvaluatorTests, FullHouseTripletHighCard) {
    std::vector<const Card*> player1_hole_cards {
        Card::getCard(Suit::Hearts, Rank::Five).get(),
        Card::getCard(Suit::Hearts, Rank::Four).get(),
    };

    std::vector<const Card*> player2_hole_cards {
        Card::getCard(Suit::Clubs, Rank::Ace).get(),
        Card::getCard(Suit::Hearts, Rank::Ace).get(),
    };

    std::vector<const Card*> community_cards {
        Card::getCard(Suit::Clubs, Rank::Queen).get(),
        Card::getCard(Suit::Spades, Rank::Queen).get(),
        Card::getCard(Suit::Spades, Rank::Ten).get(),
        Card::getCard(Suit::Clubs, Rank::Nine).get(),
        Card::getCard(Suit::Diamonds, Rank::Ace).get(),
    };

    PokerHandResult poker_hand_result = PokerHandEvaluator::determine_winner(
        player1_hole_cards, player2_hole_cards, community_cards);

    EXPECT_EQ(poker_hand_result.winner, PokerHandWinner::Player2);
    EXPECT_EQ(poker_hand_result.evaluation.category, PokerHandEvaluationCategory::FullHouse);
    EXPECT_EQ(poker_hand_result.evaluation.tiebreakers[0], (int)Rank::Ace);
    EXPECT_EQ(poker_hand_result.evaluation.tiebreakers[1], (int)Rank::Queen);
}

TEST(PokerHandEvaluatorTests, FullHousePairHighCard) {
    std::vector<const Card*> player1_hole_cards {
        Card::getCard(Suit::Hearts, Rank::Five).get(),
        Card::getCard(Suit::Hearts, Rank::Four).get(),
    };

    std::vector<const Card*> player2_hole_cards {
        Card::getCard(Suit::Clubs, Rank::Ace).get(),
        Card::getCard(Suit::Hearts, Rank::Ace).get(),
    };

    std::vector<const Card*> community_cards {
        Card::getCard(Suit::Clubs, Rank::Queen).get(),
        Card::getCard(Suit::Spades, Rank::Queen).get(),
        Card::getCard(Suit::Spades, Rank::Ten).get(),
        Card::getCard(Suit::Clubs, Rank::Nine).get(),
        Card::getCard(Suit::Diamonds, Rank::Queen).get(),
    };

    PokerHandResult poker_hand_result = PokerHandEvaluator::determine_winner(
        player1_hole_cards, player2_hole_cards, community_cards);

    EXPECT_EQ(poker_hand_result.winner, PokerHandWinner::Player2);
    EXPECT_EQ(poker_hand_result.evaluation.category, PokerHandEvaluationCategory::FullHouse);
    EXPECT_EQ(poker_hand_result.evaluation.tiebreakers[0], (int)Rank::Queen);
    EXPECT_EQ(poker_hand_result.evaluation.tiebreakers[1], (int)Rank::Ace);
}

TEST(PokerHandEvaluatorTests, Flush) {
    std::vector<const Card*> player1_hole_cards {
        Card::getCard(Suit::Hearts, Rank::Five).get(),
        Card::getCard(Suit::Hearts, Rank::Four).get(),
    };

    std::vector<const Card*> player2_hole_cards {
        Card::getCard(Suit::Clubs, Rank::Ace).get(),
        Card::getCard(Suit::Hearts, Rank::Ace).get(),
    };

    std::vector<const Card*> community_cards {
        Card::getCard(Suit::Hearts, Rank::Queen).get(),
        Card::getCard(Suit::Hearts, Rank::Jack).get(),
        Card::getCard(Suit::Hearts, Rank::Ten).get(),
        Card::getCard(Suit::Clubs, Rank::Nine).get(),
        Card::getCard(Suit::Diamonds, Rank::Queen).get(),
    };

    PokerHandResult poker_hand_result = PokerHandEvaluator::determine_winner(
        player1_hole_cards, player2_hole_cards, community_cards);

    EXPECT_EQ(poker_hand_result.winner, PokerHandWinner::Player1);
    EXPECT_EQ(poker_hand_result.evaluation.category, PokerHandEvaluationCategory::Flush);
    EXPECT_EQ(poker_hand_result.evaluation.tiebreakers[0], (int)Rank::Queen);
}

TEST(PokerHandEvaluatorTests, FlushBothHighCardKicker) {
    std::vector<const Card*> player1_hole_cards {
        Card::getCard(Suit::Hearts, Rank::Jack).get(),
        Card::getCard(Suit::Hearts, Rank::Ten).get(),
    };

    std::vector<const Card*> player2_hole_cards {
        Card::getCard(Suit::Hearts, Rank::Queen).get(),
        Card::getCard(Suit::Hearts, Rank::Three).get(),
    };

    std::vector<const Card*> community_cards {
        Card::getCard(Suit::Hearts, Rank::Two).get(),
        Card::getCard(Suit::Hearts, Rank::Four).get(),
        Card::getCard(Suit::Hearts, Rank::Seven).get(),
        Card::getCard(Suit::Clubs, Rank::Nine).get(),
        Card::getCard(Suit::Diamonds, Rank::Queen).get(),
    };

    PokerHandResult poker_hand_result = PokerHandEvaluator::determine_winner(
        player1_hole_cards, player2_hole_cards, community_cards);

    EXPECT_EQ(poker_hand_result.winner, PokerHandWinner::Player2);
    EXPECT_EQ(poker_hand_result.evaluation.category, PokerHandEvaluationCategory::Flush);
    EXPECT_EQ(poker_hand_result.evaluation.tiebreakers[0], (int)Rank::Queen);
}

TEST(PokerHandEvaluatorTests, Straight) {
    std::vector<const Card*> player1_hole_cards {
        Card::getCard(Suit::Hearts, Rank::Five).get(),
        Card::getCard(Suit::Hearts, Rank::Four).get(),
    };

    std::vector<const Card*> player2_hole_cards {
        Card::getCard(Suit::Clubs, Rank::Six).get(),
        Card::getCard(Suit::Spades, Rank::Three).get(),
    };

    std::vector<const Card*> community_cards {
        Card::getCard(Suit::Clubs, Rank::Queen).get(),
        Card::getCard(Suit::Spades, Rank::Jack).get(),
        Card::getCard(Suit::Hearts, Rank::Three).get(),
        Card::getCard(Suit::Clubs, Rank::Two).get(),
        Card::getCard(Suit::Diamonds, Rank::Ace).get(),
    };

    PokerHandResult poker_hand_result = PokerHandEvaluator::determine_winner(
        player1_hole_cards, player2_hole_cards, community_cards);

    EXPECT_EQ(poker_hand_result.winner, PokerHandWinner::Player1);
    EXPECT_EQ(poker_hand_result.evaluation.category, PokerHandEvaluationCategory::Straight);
    EXPECT_EQ(poker_hand_result.evaluation.tiebreakers[0], (int)Rank::Five);
}

TEST(PokerHandEvaluatorTests, StraightBothHighCardKicker) {
    std::vector<const Card*> player1_hole_cards {
        Card::getCard(Suit::Hearts, Rank::Five).get(),
        Card::getCard(Suit::Hearts, Rank::Four).get(),
    };

    std::vector<const Card*> player2_hole_cards {
        Card::getCard(Suit::Clubs, Rank::Six).get(),
        Card::getCard(Suit::Spades, Rank::Five).get(),
    };

    std::vector<const Card*> community_cards {
        Card::getCard(Suit::Clubs, Rank::Queen).get(),
        Card::getCard(Suit::Spades, Rank::Four).get(),
        Card::getCard(Suit::Hearts, Rank::Three).get(),
        Card::getCard(Suit::Clubs, Rank::Two).get(),
        Card::getCard(Suit::Diamonds, Rank::Ace).get(),
    };

    PokerHandResult poker_hand_result = PokerHandEvaluator::determine_winner(
        player1_hole_cards, player2_hole_cards, community_cards);

    EXPECT_EQ(poker_hand_result.winner, PokerHandWinner::Player2);
    EXPECT_EQ(poker_hand_result.evaluation.category, PokerHandEvaluationCategory::Straight);
    EXPECT_EQ(poker_hand_result.evaluation.tiebreakers[0], (int)Rank::Six);
}

TEST(PokerHandEvaluatorTests, ThreeOfAKind) {
    std::vector<const Card*> player1_hole_cards {
        Card::getCard(Suit::Hearts, Rank::Five).get(),
        Card::getCard(Suit::Hearts, Rank::Four).get(),
    };

    std::vector<const Card*> player2_hole_cards {
        Card::getCard(Suit::Clubs, Rank::Six).get(),
        Card::getCard(Suit::Spades, Rank::Five).get(),
    };

    std::vector<const Card*> community_cards {
        Card::getCard(Suit::Clubs, Rank::Queen).get(),
        Card::getCard(Suit::Spades, Rank::Four).get(),
        Card::getCard(Suit::Clubs, Rank::Four).get(),
        Card::getCard(Suit::Clubs, Rank::Jack).get(),
        Card::getCard(Suit::Diamonds, Rank::Ace).get(),
    };

    PokerHandResult poker_hand_result = PokerHandEvaluator::determine_winner(
        player1_hole_cards, player2_hole_cards, community_cards);

    EXPECT_EQ(poker_hand_result.winner, PokerHandWinner::Player1);
    EXPECT_EQ(poker_hand_result.evaluation.category, PokerHandEvaluationCategory::ThreeOfAKind);
    EXPECT_EQ(poker_hand_result.evaluation.tiebreakers[0], (int)Rank::Four);
    EXPECT_EQ(poker_hand_result.evaluation.tiebreakers[1], (int)Rank::Ace);
}

TEST(PokerHandEvaluatorTests, ThreeOfAKindBothHighCardKicker) {
    std::vector<const Card*> player1_hole_cards {
        Card::getCard(Suit::Hearts, Rank::Five).get(),
        Card::getCard(Suit::Hearts, Rank::Four).get(),
    };

    std::vector<const Card*> player2_hole_cards {
        Card::getCard(Suit::Clubs, Rank::Queen).get(),
        Card::getCard(Suit::Spades, Rank::Four).get(),
    };

    std::vector<const Card*> community_cards {
        Card::getCard(Suit::Clubs, Rank::Nine).get(),
        Card::getCard(Suit::Diamonds, Rank::Four).get(),
        Card::getCard(Suit::Clubs, Rank::Four).get(),
        Card::getCard(Suit::Clubs, Rank::Three).get(),
        Card::getCard(Suit::Diamonds, Rank::Two).get(),
    };

    PokerHandResult poker_hand_result = PokerHandEvaluator::determine_winner(
        player1_hole_cards, player2_hole_cards, community_cards);

    EXPECT_EQ(poker_hand_result.winner, PokerHandWinner::Player2);
    EXPECT_EQ(poker_hand_result.evaluation.category, PokerHandEvaluationCategory::ThreeOfAKind);
    EXPECT_EQ(poker_hand_result.evaluation.tiebreakers[0], (int)Rank::Four);
    EXPECT_EQ(poker_hand_result.evaluation.tiebreakers[1], (int)Rank::Queen);
}

TEST(PokerHandEvaluatorTests, TwoPair) {
    std::vector<const Card*> player1_hole_cards {
        Card::getCard(Suit::Hearts, Rank::Five).get(),
        Card::getCard(Suit::Hearts, Rank::Four).get(),
    };

    std::vector<const Card*> player2_hole_cards {
        Card::getCard(Suit::Clubs, Rank::Queen).get(),
        Card::getCard(Suit::Spades, Rank::Three).get(),
    };

    std::vector<const Card*> community_cards {
        Card::getCard(Suit::Clubs, Rank::Five).get(),
        Card::getCard(Suit::Diamonds, Rank::Four).get(),
        Card::getCard(Suit::Clubs, Rank::Jack).get(),
        Card::getCard(Suit::Clubs, Rank::Three).get(),
        Card::getCard(Suit::Diamonds, Rank::Two).get(),
    };

    PokerHandResult poker_hand_result = PokerHandEvaluator::determine_winner(
        player1_hole_cards, player2_hole_cards, community_cards);

    EXPECT_EQ(poker_hand_result.winner, PokerHandWinner::Player1);
    EXPECT_EQ(poker_hand_result.evaluation.category, PokerHandEvaluationCategory::TwoPair);
    EXPECT_EQ(poker_hand_result.evaluation.tiebreakers[0], (int)Rank::Five);
    EXPECT_EQ(poker_hand_result.evaluation.tiebreakers[1], (int)Rank::Four);
}

TEST(PokerHandEvaluatorTests, OnePair) {
    std::vector<const Card*> player1_hole_cards {
        Card::getCard(Suit::Hearts, Rank::Five).get(),
        Card::getCard(Suit::Hearts, Rank::King).get(),
    };

    std::vector<const Card*> player2_hole_cards {
        Card::getCard(Suit::Clubs, Rank::Queen).get(),
        Card::getCard(Suit::Spades, Rank::Eight).get(),
    };

    std::vector<const Card*> community_cards {
        Card::getCard(Suit::Clubs, Rank::Five).get(),
        Card::getCard(Suit::Diamonds, Rank::Seven).get(),
        Card::getCard(Suit::Clubs, Rank::Jack).get(),
        Card::getCard(Suit::Clubs, Rank::Three).get(),
        Card::getCard(Suit::Diamonds, Rank::Two).get(),
    };

    PokerHandResult poker_hand_result = PokerHandEvaluator::determine_winner(
        player1_hole_cards, player2_hole_cards, community_cards);

    EXPECT_EQ(poker_hand_result.winner, PokerHandWinner::Player1);
    EXPECT_EQ(poker_hand_result.evaluation.category, PokerHandEvaluationCategory::OnePair);
    EXPECT_EQ(poker_hand_result.evaluation.tiebreakers[0], (int)Rank::Five);
    EXPECT_EQ(poker_hand_result.evaluation.tiebreakers[1], (int)Rank::King);
}

TEST(PokerHandEvaluatorTests, HighCard) {
    std::vector<const Card*> player1_hole_cards {
        Card::getCard(Suit::Hearts, Rank::Five).get(),
        Card::getCard(Suit::Hearts, Rank::King).get(),
    };

    std::vector<const Card*> player2_hole_cards {
        Card::getCard(Suit::Clubs, Rank::Queen).get(),
        Card::getCard(Suit::Spades, Rank::Eight).get(),
    };

    std::vector<const Card*> community_cards {
        Card::getCard(Suit::Clubs, Rank::Nine).get(),
        Card::getCard(Suit::Diamonds, Rank::Seven).get(),
        Card::getCard(Suit::Clubs, Rank::Jack).get(),
        Card::getCard(Suit::Clubs, Rank::Three).get(),
        Card::getCard(Suit::Diamonds, Rank::Two).get(),
    };

    PokerHandResult poker_hand_result = PokerHandEvaluator::determine_winner(
        player1_hole_cards, player2_hole_cards, community_cards);

    EXPECT_EQ(poker_hand_result.winner, PokerHandWinner::Player1);
    EXPECT_EQ(poker_hand_result.evaluation.category, PokerHandEvaluationCategory::HighCard);
    EXPECT_EQ(poker_hand_result.evaluation.tiebreakers[0], (int)Rank::King);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
