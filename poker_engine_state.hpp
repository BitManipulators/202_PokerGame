#pragma once

#include "poker_game.hpp"
#include "game_constants.hpp"

#include <cstddef>

class PokerEngineStatesBlock;

class PokerEngineState {
public:
    PokerEngineState(PokerGame& game,
                     PokerEngineStatesBlock& states,
                     PokerEngineEnumState enum_state)
        : game_(game)
        , states_(states)
        , enum_state_(enum_state) {}
    virtual ~PokerEngineState() = default;

    virtual std::tuple<PokerEngineState*, GameAction::Result> new_game();
    virtual std::tuple<PokerEngineState*, GameAction::Result> fold(PlayerType player_type);
    virtual std::tuple<PokerEngineState*, GameAction::Result> call(PlayerType player_type);
    virtual std::tuple<PokerEngineState*, GameAction::Result> raise(PlayerType player_type, const std::size_t value);
    virtual std::tuple<PokerEngineState*, GameAction::Result> transition_state() = 0;

protected:
    PokerGame& game_;
    PokerEngineStatesBlock& states_;

public:
    const PokerEngineEnumState enum_state_;
};

class PreFlopState : public PokerEngineState {
public:
    PreFlopState(PokerEngineStatesBlock& states, PokerGame& game) :
        PokerEngineState(game, states, PokerEngineEnumState::PreFlop) {}

    std::tuple<PokerEngineState*, GameAction::Result> transition_state() override;
};

class FlopState : public PokerEngineState {
public:
    FlopState(PokerEngineStatesBlock& states, PokerGame& game) :
        PokerEngineState(game, states, PokerEngineEnumState::Flop) {}

    std::tuple<PokerEngineState*, GameAction::Result> transition_state() override;
};

class TurnState : public PokerEngineState {
public:
    TurnState(PokerEngineStatesBlock& states, PokerGame& game) :
        PokerEngineState(game, states, PokerEngineEnumState::Turn) {}

    std::tuple<PokerEngineState*, GameAction::Result> transition_state() override;
};

class RiverState : public PokerEngineState {
public:
    RiverState(PokerEngineStatesBlock& states, PokerGame& game) :
        PokerEngineState(game, states, PokerEngineEnumState::River) {}

    std::tuple<PokerEngineState*, GameAction::Result> transition_state() override;
};

class ShowdownState : public PokerEngineState {
public:
    ShowdownState(PokerEngineStatesBlock& states, PokerGame& game) :
        PokerEngineState(game, states, PokerEngineEnumState::Showdown) {}

    std::tuple<PokerEngineState*, GameAction::Result> new_game() override;
    std::tuple<PokerEngineState*, GameAction::Result> fold(PlayerType player_type) override;
    std::tuple<PokerEngineState*, GameAction::Result> call(PlayerType player_type) override;
    std::tuple<PokerEngineState*, GameAction::Result> raise(PlayerType player_type, const std::size_t value) override;
    std::tuple<PokerEngineState*, GameAction::Result> transition_state() override;
};

class FoldedState : public PokerEngineState {
public:
    FoldedState(PokerEngineStatesBlock& states, PokerGame& game) :
        PokerEngineState(game, states, PokerEngineEnumState::Folded) {}

    std::tuple<PokerEngineState*, GameAction::Result> new_game() override;
    std::tuple<PokerEngineState*, GameAction::Result> fold(PlayerType player_type) override;
    std::tuple<PokerEngineState*, GameAction::Result> call(PlayerType player_type) override;
    std::tuple<PokerEngineState*, GameAction::Result> raise(PlayerType player_type, const std::size_t value) override;
    std::tuple<PokerEngineState*, GameAction::Result> transition_state() override;
};

class PokerEngineStatesBlock {
public:
    PokerEngineStatesBlock(PokerGame& game)
        : game_(game) {
        PREFLOP_STATE = new PreFlopState(*this, game);
        FLOP_STATE = new FlopState(*this, game);
        TURN_STATE = new TurnState(*this, game);
        RIVER_STATE = new RiverState(*this, game);
        SHOWDOWN_STATE = new ShowdownState(*this, game);
        FOLDED_STATE = new FoldedState(*this, game);
    }

    ~PokerEngineStatesBlock() {
        delete PREFLOP_STATE;
        delete FLOP_STATE;
        delete TURN_STATE;
        delete RIVER_STATE;
        delete SHOWDOWN_STATE;
        delete FOLDED_STATE;
    }

    std::tuple<PokerEngineState*, GameAction::Result> initial_state();

    friend PokerEngineState;
    friend PreFlopState;
    friend FlopState;
    friend TurnState;
    friend RiverState;
    friend ShowdownState;
    friend FoldedState;

private:
    PokerGame& game_;

    PreFlopState* PREFLOP_STATE;
    FlopState* FLOP_STATE;
    TurnState* TURN_STATE;
    RiverState* RIVER_STATE;
    ShowdownState* SHOWDOWN_STATE;
    FoldedState* FOLDED_STATE;
};
