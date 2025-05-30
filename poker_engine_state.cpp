#include "poker_engine_state.hpp"


namespace {

std::string to_string(PlayerType player_type) {
    switch(player_type) {
    case PlayerType::Human:
        return "human";
    case PlayerType::Computer:
    default:
        return "computer";
    }
}

std::string to_string(PokerEngineEnumState enum_state) {
    switch(enum_state) {
    case PokerEngineEnumState::PreFlop:
        return "PreFlop";
    case PokerEngineEnumState::Flop:
        return "Flop";
    case PokerEngineEnumState::Turn:
        return "Turn";
    case PokerEngineEnumState::River:
        return "River";
    case PokerEngineEnumState::Showdown:
        return "Showdown";
    case PokerEngineEnumState::Folded:
    default:
        return "Folded";
    }
}

std::tuple<PokerEngineState*, GameAction::Result> transition_to_preflop(PokerEngineState* preflop_state, PokerGame& game) {

    game.prepare_new_game();
    game.clear_player_actions();
    game.rotate_dealer();
    game.shuffle_deck();
    game.post_blinds();
    game.deal_hole_cards();

    return {preflop_state, GameAction::OK};
}

} // namespace

std::tuple<PokerEngineState*, GameAction::Result> PokerEngineStatesBlock::initial_state() {
    return transition_to_preflop(PREFLOP_STATE, game_);
}

std::tuple<PokerEngineState*, GameAction::Result> PokerEngineState::new_game() {
    return {this, GameAction::OK};
}

std::tuple<PokerEngineState*, GameAction::Result> PokerEngineState::fold(PlayerType player_type) {

    GameAction::Result result = game_.perform_fold(player_type);
    if (!result.ok) {
        return {this, result};
    }

    return {states_.FOLDED_STATE, GameAction::OK};
}

std::tuple<PokerEngineState*, GameAction::Result> PokerEngineState::call(PlayerType player_type) {

    GameAction::Result result = game_.perform_call(player_type);
    if (!result.ok) {
        return {this, result};
    }

    if (game_.all_players_have_acted()) {
        return transition_state();
    }

    return {this, GameAction::OK};
}

std::tuple<PokerEngineState*, GameAction::Result> PokerEngineState::raise(PlayerType player_type, const std::size_t raise) {

    GameAction::Result result = game_.perform_raise(player_type, raise);

    return {this, result};
}

std::tuple<PokerEngineState*, GameAction::Result> PreFlopState::transition_state() {
    std::string from = to_string(this->enum_state_);
    std::string to = to_string(states_.FLOP_STATE->enum_state_);
    game_.notify_game_event(std::make_shared<StateTransitionEvent>(from, to));

    game_.clear_player_actions();
    game_.deal_flop();

    return {states_.FLOP_STATE, GameAction::OK};
}

std::tuple<PokerEngineState*, GameAction::Result> FlopState::transition_state() {
    std::string from = to_string(this->enum_state_);
    std::string to = to_string(states_.TURN_STATE->enum_state_);
    game_.notify_game_event(std::make_shared<StateTransitionEvent>(from, to));

    game_.clear_player_actions();
    game_.deal_turn();

    return {states_.TURN_STATE, GameAction::OK};
}

std::tuple<PokerEngineState*, GameAction::Result> TurnState::transition_state() {
    std::string from = to_string(this->enum_state_);
    std::string to = to_string(states_.RIVER_STATE->enum_state_);
    game_.notify_game_event(std::make_shared<StateTransitionEvent>(from, to));

    game_.clear_player_actions();
    game_.deal_river();

    return {states_.RIVER_STATE, GameAction::OK};
}

std::tuple<PokerEngineState*, GameAction::Result> RiverState::transition_state() {
    std::string from = to_string(this->enum_state_);
    std::string to = to_string(states_.SHOWDOWN_STATE->enum_state_);
    game_.notify_game_event(std::make_shared<StateTransitionEvent>(from, to));

    game_.clear_player_actions();
    game_.determine_winner();

    return {states_.SHOWDOWN_STATE, GameAction::OK};
}

std::tuple<PokerEngineState*, GameAction::Result> ShowdownState::new_game() {
    return transition_to_preflop(states_.PREFLOP_STATE, game_);
}

std::tuple<PokerEngineState*, GameAction::Result> ShowdownState::fold(PlayerType player_type) {
    return {this, GameAction::ERROR("Fold called in Showdown state!")};
}

std::tuple<PokerEngineState*, GameAction::Result> ShowdownState::call(PlayerType player_type) {
    return {this, GameAction::ERROR("Call called in Showdown state!")};
}

std::tuple<PokerEngineState*, GameAction::Result> ShowdownState::raise(PlayerType player_type, const std::size_t raise) {
    return {this, GameAction::ERROR("Raise called in Showdown state!")};
}

std::tuple<PokerEngineState*, GameAction::Result> ShowdownState::transition_state() {
    return {this, GameAction::ERROR("Transition state called in Showdown state!")};
}

std::tuple<PokerEngineState*, GameAction::Result> FoldedState::new_game() {
    return transition_to_preflop(states_.PREFLOP_STATE, game_);
}

std::tuple<PokerEngineState*, GameAction::Result> FoldedState::fold(PlayerType player_type) {
    return {this, GameAction::ERROR("Fold called in Folded state!")};
}

std::tuple<PokerEngineState*, GameAction::Result> FoldedState::call(PlayerType player_type) {
    return {this, GameAction::ERROR("Call called in Folded state!")};
}

std::tuple<PokerEngineState*, GameAction::Result> FoldedState::raise(PlayerType player_type, const std::size_t raise) {
    return {this, GameAction::ERROR("Raise called in Folded state!")};
}

std::tuple<PokerEngineState*, GameAction::Result> FoldedState::transition_state() {
    return {this, GameAction::ERROR("Transition state called in Folded state!")};
}
