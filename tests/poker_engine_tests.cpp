#include "../poker_engine.hpp"

#include <gtest/gtest.h>

class PokerEngineTests : public ::testing::Test {
protected:
    PokerGame* game() {
        return engine.game;
    }

    PokerEngineState* state() {
        return engine.state;
    }

    PokerEngineEnumState enum_state() {
        return engine.state->enum_state_;
    }

    PokerEngineStatesBlock* block() {
        return engine.block;
    }

    void advance_to_flop() {
        GameAction::Result result = engine.make_move(PlayerType::Human, Call{});
        EXPECT_TRUE(result.ok);
        EXPECT_FALSE(game()->has_ended());
        EXPECT_TRUE(enum_state() == PokerEngineEnumState::PreFlop);

        result = engine.make_move(PlayerType::Computer, Call{});
        EXPECT_TRUE(result.ok);
        EXPECT_FALSE(game()->has_ended());
        EXPECT_TRUE(enum_state() == PokerEngineEnumState::Flop);
    }

    void advance_to_turn() {
        advance_to_flop();

        GameAction::Result result = engine.make_move(PlayerType::Human, Call{});
        EXPECT_TRUE(result.ok);
        EXPECT_FALSE(game()->has_ended());
        EXPECT_TRUE(enum_state() == PokerEngineEnumState::Flop);

        result = engine.make_move(PlayerType::Computer, Call{});
        EXPECT_TRUE(result.ok);
        EXPECT_FALSE(game()->has_ended());
        EXPECT_TRUE(enum_state() == PokerEngineEnumState::Turn);
    }

    void advance_to_river() {
        advance_to_turn();

        GameAction::Result result = engine.make_move(PlayerType::Human, Call{});
        EXPECT_TRUE(result.ok);
        EXPECT_FALSE(game()->has_ended());
        EXPECT_TRUE(enum_state() == PokerEngineEnumState::Turn);

        result = engine.make_move(PlayerType::Computer, Call{});
        EXPECT_TRUE(result.ok);
        EXPECT_FALSE(game()->has_ended());
        EXPECT_TRUE(enum_state() == PokerEngineEnumState::River);
    }

    void advance_to_showdown() {
        advance_to_river();

        GameAction::Result result = engine.make_move(PlayerType::Human, Call{});
        EXPECT_TRUE(result.ok);
        EXPECT_FALSE(game()->has_ended());
        EXPECT_TRUE(enum_state() == PokerEngineEnumState::River);

        result = engine.make_move(PlayerType::Computer, Call{});
        EXPECT_TRUE(result.ok);
        EXPECT_TRUE(game()->has_ended());
        EXPECT_TRUE(enum_state() == PokerEngineEnumState::Showdown);
    }

    PokerEngine engine;
};

TEST_F(PokerEngineTests, FoldPreFlop) {
    std::size_t starting_pot = game()->get_pot();
    GameAction::Result result = engine.make_move(PlayerType::Human, Fold{});
    EXPECT_TRUE(result.ok);
    EXPECT_TRUE(game()->has_ended());
    EXPECT_TRUE(enum_state() == PokerEngineEnumState::Folded);
    EXPECT_EQ(game()->get_pot(), starting_pot);
}

TEST_F(PokerEngineTests, CallPreFlop) {
    std::size_t starting_pot = game()->get_pot();
    GameAction::Result result = engine.make_move(PlayerType::Human, Call{});
    EXPECT_TRUE(result.ok);
    EXPECT_FALSE(game()->has_ended());
    EXPECT_TRUE(enum_state() == PokerEngineEnumState::PreFlop);
    EXPECT_EQ(game()->get_pot(), starting_pot);
}

TEST_F(PokerEngineTests, RaisePreFlop) {
    std::size_t starting_pot = game()->get_pot();
    GameAction::Result result = engine.make_move(PlayerType::Human, Raise{100});
    EXPECT_TRUE(result.ok);
    EXPECT_FALSE(game()->has_ended());
    EXPECT_TRUE(enum_state() == PokerEngineEnumState::PreFlop);
    EXPECT_EQ(game()->get_pot(), starting_pot + 100);
}

TEST_F(PokerEngineTests, CallAndCallPreFlop) {
    std::size_t starting_pot = game()->get_pot();
    std::size_t human_starting_chips = game()->get_human_player().chips;
    std::size_t computer_starting_chips = game()->get_computer_player().chips;

    GameAction::Result result = engine.make_move(PlayerType::Human, Call{});
    EXPECT_TRUE(result.ok);
    EXPECT_FALSE(game()->has_ended());
    EXPECT_TRUE(enum_state() == PokerEngineEnumState::PreFlop);

    result = engine.make_move(PlayerType::Computer, Call{});
    EXPECT_TRUE(result.ok);
    EXPECT_FALSE(game()->has_ended());
    EXPECT_TRUE(enum_state() == PokerEngineEnumState::Flop);

    EXPECT_EQ(game()->get_human_player().chips, human_starting_chips);
    EXPECT_EQ(game()->get_computer_player().chips, computer_starting_chips);
    EXPECT_EQ(game()->get_pot(), starting_pot);
}

TEST_F(PokerEngineTests, RaiseAndCallPreFlop) {
    std::size_t starting_pot = game()->get_pot();
    std::size_t human_starting_chips = game()->get_human_player().chips;
    std::size_t computer_starting_chips = game()->get_computer_player().chips;

    GameAction::Result result = engine.make_move(PlayerType::Human, Raise{100});
    EXPECT_TRUE(result.ok);
    EXPECT_FALSE(game()->has_ended());
    EXPECT_TRUE(enum_state() == PokerEngineEnumState::PreFlop);

    result = engine.make_move(PlayerType::Computer, Call{});
    EXPECT_TRUE(result.ok);
    EXPECT_FALSE(game()->has_ended());
    EXPECT_TRUE(enum_state() == PokerEngineEnumState::Flop);

    EXPECT_EQ(game()->get_human_player().chips, human_starting_chips - 100);
    EXPECT_EQ(game()->get_computer_player().chips, computer_starting_chips - 100);
    EXPECT_EQ(game()->get_pot(), starting_pot + 200);
}

TEST_F(PokerEngineTests, RaiseAndRaisePreFlop) {
    std::size_t starting_pot = game()->get_pot();
    std::size_t human_starting_chips = game()->get_human_player().chips;
    std::size_t computer_starting_chips = game()->get_computer_player().chips;

    GameAction::Result result = engine.make_move(PlayerType::Human, Raise{100});
    EXPECT_TRUE(result.ok);
    EXPECT_FALSE(game()->has_ended());
    EXPECT_TRUE(enum_state() == PokerEngineEnumState::PreFlop);

    result = engine.make_move(PlayerType::Computer, Raise{200});
    EXPECT_TRUE(result.ok);
    EXPECT_FALSE(game()->has_ended());
    EXPECT_TRUE(enum_state() == PokerEngineEnumState::PreFlop);

    EXPECT_EQ(game()->get_human_player().chips, human_starting_chips - 100);
    EXPECT_EQ(game()->get_computer_player().chips, computer_starting_chips - 200);
    EXPECT_EQ(game()->get_pot(), starting_pot + 300);
}

TEST_F(PokerEngineTests, RaiseAndRaiseAndFoldPreFlop) {
    std::size_t starting_pot = game()->get_pot();
    std::size_t human_starting_chips = game()->get_human_player().chips;
    std::size_t computer_starting_chips = game()->get_computer_player().chips;

    GameAction::Result result = engine.make_move(PlayerType::Human, Raise{100});
    EXPECT_TRUE(result.ok);
    EXPECT_FALSE(game()->has_ended());
    EXPECT_TRUE(enum_state() == PokerEngineEnumState::PreFlop);

    result = engine.make_move(PlayerType::Computer, Raise{200});
    EXPECT_TRUE(result.ok);
    EXPECT_FALSE(game()->has_ended());
    EXPECT_TRUE(enum_state() == PokerEngineEnumState::PreFlop);

    result = engine.make_move(PlayerType::Human, Fold{});
    EXPECT_TRUE(result.ok);
    EXPECT_TRUE(game()->has_ended());
    EXPECT_TRUE(enum_state() == PokerEngineEnumState::Folded);

    EXPECT_EQ(game()->get_human_player().chips, human_starting_chips - 100);
    EXPECT_EQ(game()->get_computer_player().chips, computer_starting_chips + game()->get_pot() - 200);
    EXPECT_EQ(game()->get_pot(), starting_pot + 300);
}

TEST_F(PokerEngineTests, RaiseAndRaiseAndCallPreFlop) {
    std::size_t starting_pot = game()->get_pot();
    std::size_t human_starting_chips = game()->get_human_player().chips;
    std::size_t computer_starting_chips = game()->get_computer_player().chips;

    GameAction::Result result = engine.make_move(PlayerType::Human, Raise{100});
    EXPECT_TRUE(result.ok);
    EXPECT_FALSE(game()->has_ended());
    EXPECT_TRUE(enum_state() == PokerEngineEnumState::PreFlop);


    result = engine.make_move(PlayerType::Computer, Raise{200});
    EXPECT_TRUE(result.ok);
    EXPECT_FALSE(game()->has_ended());
    EXPECT_TRUE(enum_state() == PokerEngineEnumState::PreFlop);

    result = engine.make_move(PlayerType::Human, Call{});
    EXPECT_TRUE(result.ok);
    EXPECT_FALSE(game()->has_ended());
    EXPECT_TRUE(enum_state() == PokerEngineEnumState::Flop);

    EXPECT_EQ(game()->get_human_player().chips, human_starting_chips - 200);
    EXPECT_EQ(game()->get_computer_player().chips, computer_starting_chips - 200);
    EXPECT_EQ(game()->get_pot(), starting_pot + 400);
}

TEST_F(PokerEngineTests, RaiseAndRaiseAndRaisePreFlop) {
    std::size_t starting_pot = game()->get_pot();
    std::size_t human_starting_chips = game()->get_human_player().chips;
    std::size_t computer_starting_chips = game()->get_computer_player().chips;

    GameAction::Result result = engine.make_move(PlayerType::Human, Raise{100});
    EXPECT_TRUE(result.ok);
    EXPECT_FALSE(game()->has_ended());
    EXPECT_TRUE(enum_state() == PokerEngineEnumState::PreFlop);

    result = engine.make_move(PlayerType::Computer, Raise{200});
    EXPECT_TRUE(result.ok);
    EXPECT_FALSE(game()->has_ended());
    EXPECT_TRUE(enum_state() == PokerEngineEnumState::PreFlop);

    result = engine.make_move(PlayerType::Human, Raise{400});
    EXPECT_TRUE(result.ok);
    EXPECT_FALSE(game()->has_ended());
    EXPECT_TRUE(enum_state() == PokerEngineEnumState::PreFlop);

    EXPECT_EQ(game()->get_human_player().chips, human_starting_chips - 400);
    EXPECT_EQ(game()->get_computer_player().chips, computer_starting_chips - 200);
    EXPECT_EQ(game()->get_pot(), starting_pot + 600);
}

TEST_F(PokerEngineTests, CallAndCallFlop) {
    advance_to_flop();

    std::size_t starting_pot = game()->get_pot();
    std::size_t human_starting_chips = game()->get_human_player().chips;
    std::size_t computer_starting_chips = game()->get_computer_player().chips;

    GameAction::Result result = engine.make_move(PlayerType::Human, Call{});
    EXPECT_TRUE(result.ok);
    EXPECT_FALSE(game()->has_ended());
    EXPECT_TRUE(enum_state() == PokerEngineEnumState::Flop);

    result = engine.make_move(PlayerType::Computer, Call{});
    EXPECT_TRUE(result.ok);
    EXPECT_FALSE(game()->has_ended());
    EXPECT_TRUE(enum_state() == PokerEngineEnumState::Turn);

    EXPECT_EQ(game()->get_human_player().chips, human_starting_chips);
    EXPECT_EQ(game()->get_computer_player().chips, computer_starting_chips);
    EXPECT_EQ(game()->get_pot(), starting_pot);
}

TEST_F(PokerEngineTests, RaiseAndFoldFlop) {
    advance_to_flop();

    std::size_t starting_pot = game()->get_pot();
    std::size_t human_starting_chips = game()->get_human_player().chips;
    std::size_t computer_starting_chips = game()->get_computer_player().chips;

    GameAction::Result result = engine.make_move(PlayerType::Human, Raise{100});
    EXPECT_TRUE(result.ok);
    EXPECT_FALSE(game()->has_ended());
    EXPECT_TRUE(enum_state() == PokerEngineEnumState::Flop);

    result = engine.make_move(PlayerType::Computer, Fold{});
    EXPECT_TRUE(result.ok);
    EXPECT_TRUE(game()->has_ended());
    EXPECT_TRUE(enum_state() == PokerEngineEnumState::Folded);

    EXPECT_EQ(game()->get_human_player().chips, human_starting_chips + starting_pot);
    EXPECT_EQ(game()->get_computer_player().chips, computer_starting_chips);
    EXPECT_EQ(game()->get_pot(), starting_pot + 100);
}

TEST_F(PokerEngineTests, RaiseAndCallFlop) {
    advance_to_flop();

    std::size_t starting_pot = game()->get_pot();
    std::size_t human_starting_chips = game()->get_human_player().chips;
    std::size_t computer_starting_chips = game()->get_computer_player().chips;

    GameAction::Result result = engine.make_move(PlayerType::Human, Raise{100});
    EXPECT_TRUE(result.ok);
    EXPECT_FALSE(game()->has_ended());
    EXPECT_TRUE(enum_state() == PokerEngineEnumState::Flop);

    result = engine.make_move(PlayerType::Computer, Call{});
    EXPECT_TRUE(result.ok);
    EXPECT_FALSE(game()->has_ended());
    EXPECT_TRUE(enum_state() == PokerEngineEnumState::Turn);

    EXPECT_EQ(game()->get_human_player().chips, human_starting_chips - 100);
    EXPECT_EQ(game()->get_computer_player().chips, computer_starting_chips - 100);
    EXPECT_EQ(game()->get_pot(), starting_pot + 200);
}

TEST_F(PokerEngineTests, CallAndRaiseAndCallFlop) {
    advance_to_flop();

    std::size_t starting_pot = game()->get_pot();
    std::size_t human_starting_chips = game()->get_human_player().chips;
    std::size_t computer_starting_chips = game()->get_computer_player().chips;

    GameAction::Result result = engine.make_move(PlayerType::Human, Call{});
    EXPECT_TRUE(result.ok);
    EXPECT_FALSE(game()->has_ended());
    EXPECT_TRUE(enum_state() == PokerEngineEnumState::Flop);

    result = engine.make_move(PlayerType::Computer, Raise{100});
    EXPECT_TRUE(result.ok);
    EXPECT_FALSE(game()->has_ended());
    EXPECT_TRUE(enum_state() == PokerEngineEnumState::Flop);

    result = engine.make_move(PlayerType::Human, Call{});
    EXPECT_TRUE(result.ok);
    EXPECT_FALSE(game()->has_ended());
    EXPECT_TRUE(enum_state() == PokerEngineEnumState::Turn);

    EXPECT_EQ(game()->get_human_player().chips, human_starting_chips - 100);
    EXPECT_EQ(game()->get_computer_player().chips, computer_starting_chips - 100);
    EXPECT_EQ(game()->get_pot(), starting_pot + 200);
}

TEST_F(PokerEngineTests, CallAndRaiseAndFoldFlop) {
    advance_to_flop();

    std::size_t starting_pot = game()->get_pot();
    std::size_t human_starting_chips = game()->get_human_player().chips;
    std::size_t computer_starting_chips = game()->get_computer_player().chips;

    GameAction::Result result = engine.make_move(PlayerType::Human, Call{});
    EXPECT_TRUE(result.ok);
    EXPECT_FALSE(game()->has_ended());
    EXPECT_TRUE(enum_state() == PokerEngineEnumState::Flop);

    result = engine.make_move(PlayerType::Computer, Raise{100});
    EXPECT_TRUE(result.ok);
    EXPECT_FALSE(game()->has_ended());
    EXPECT_TRUE(enum_state() == PokerEngineEnumState::Flop);

    result = engine.make_move(PlayerType::Human, Fold{});
    EXPECT_TRUE(result.ok);
    EXPECT_TRUE(game()->has_ended());
    EXPECT_TRUE(enum_state() == PokerEngineEnumState::Folded);

    EXPECT_EQ(game()->get_human_player().chips, human_starting_chips);
    EXPECT_EQ(game()->get_computer_player().chips, computer_starting_chips + starting_pot);
    EXPECT_EQ(game()->get_human_player().current_bet, 0);
    EXPECT_EQ(game()->get_computer_player().current_bet, 100);
    EXPECT_EQ(game()->get_pot(), starting_pot + 100);
}

TEST_F(PokerEngineTests, CallAndCallRiver) {
    advance_to_river();

    std::size_t starting_pot = game()->get_pot();
    std::size_t human_starting_chips = game()->get_human_player().chips;
    std::size_t computer_starting_chips = game()->get_computer_player().chips;

    GameAction::Result result = engine.make_move(PlayerType::Human, Call{});
    EXPECT_TRUE(result.ok);
    EXPECT_FALSE(game()->has_ended());
    EXPECT_TRUE(enum_state() == PokerEngineEnumState::River);

    result = engine.make_move(PlayerType::Computer, Call{});
    EXPECT_TRUE(result.ok);
    EXPECT_TRUE(game()->has_ended());
    EXPECT_TRUE(enum_state() == PokerEngineEnumState::Showdown);

    // EXPECT_EQ(game()->get_human_player().chips, human_starting_chips);
    // EXPECT_EQ(game()->get_computer_player().chips, computer_starting_chips);
    EXPECT_EQ(game()->get_pot(), starting_pot);
}

TEST_F(PokerEngineTests, CallShowdown) {
    advance_to_showdown();

    GameAction::Result result = engine.make_move(PlayerType::Human, Call{});
    EXPECT_FALSE(result.ok);
    EXPECT_TRUE(game()->has_ended());
    EXPECT_TRUE(enum_state() == PokerEngineEnumState::Showdown);

    result = engine.make_move(PlayerType::Computer, Call{});
    EXPECT_FALSE(result.ok);
    EXPECT_TRUE(game()->has_ended());
    EXPECT_TRUE(enum_state() == PokerEngineEnumState::Showdown);
}

TEST_F(PokerEngineTests, NewGameAfterFolded) {
    GameAction::Result result = engine.make_move(PlayerType::Human, Fold{});
    EXPECT_TRUE(result.ok);
    EXPECT_TRUE(enum_state() == PokerEngineEnumState::Folded);
    EXPECT_TRUE(game()->has_ended());

    result = engine.new_game();
    EXPECT_TRUE(result.ok);
    EXPECT_TRUE(enum_state() == PokerEngineEnumState::PreFlop);
    EXPECT_FALSE(game()->has_ended());

    EXPECT_EQ(game()->get_human_player().current_bet, 0);
    EXPECT_EQ(game()->get_computer_player().current_bet, 0);
}

TEST_F(PokerEngineTests, NewGameAfterShowdown) {
    advance_to_showdown();

    GameAction::Result result = engine.new_game();
    EXPECT_TRUE(result.ok);
    EXPECT_TRUE(enum_state() == PokerEngineEnumState::PreFlop);
    EXPECT_FALSE(game()->has_ended());
    EXPECT_EQ(game()->get_human_player().current_bet, 0);
    EXPECT_EQ(game()->get_computer_player().current_bet, 0);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
