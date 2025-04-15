#include "poker_engine.hpp"

#include "poker_engine_state.hpp"

PokerEngine::PokerEngine() {
    game = new PokerGame();
    block = new PokerEngineStatesBlock(*game);
    auto [initial_state, _] = block->initial_state();
    state = initial_state;
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

GameAction::Result PokerEngine::make_move(PlayerType player_type, Move move) {
    return std::visit([this, player_type](const auto& m) {
        using T = std::decay_t<decltype(m)>;

        if constexpr (std::is_same_v<T, Fold>) {
            auto [fold_state, fold_result] = state->fold(player_type);
            state = fold_state;
            return fold_result;
        } else if constexpr (std::is_same_v<T, Call>) {
            auto [call_state, call_result] = state->call(player_type);
            state = call_state;
            return call_result;
        } else if constexpr (std::is_same_v<T, Raise>) {
            auto [raise_state, raise_result] = state->raise(player_type, m.amount);
            state = raise_state;
            return raise_result;
        }
    }, move);
}

const PokerGame& PokerEngine::get_game() {
    return *game;
}
