#include "poker_engine.hpp"

#include "poker_engine_state.hpp"

PokerEngine::PokerEngine() {
    game = new PokerGame();
    block = new PokerEngineStatesBlock(*game);
    auto [new_state, _] = block->initial_state();
    state = new_state;
}

PokerEngine::~PokerEngine() {
    delete game;
    delete block;
}

GameAction::Result PokerEngine::new_game() {
    auto [new_game_state, action_result] = state->new_game();
    state = new_game_state;
    return action_result;
}

GameAction::Result PokerEngine::call(PlayerType player_type) {
    auto [call_state, action_result] = state->call(player_type);
    state = call_state;
    return action_result;
}

GameAction::Result PokerEngine::fold(PlayerType player_type) {
    auto [fold_state, action_result] = state->fold(player_type);
    state = fold_state;
    return action_result;
}

GameAction::Result PokerEngine::raise(PlayerType player_type, const std::size_t value) {
    auto [raise_state, action_result] = state->raise(player_type, value);
    state = raise_state;
    return action_result;
}

const PokerGame& PokerEngine::get_game() {
    return *game;
}
