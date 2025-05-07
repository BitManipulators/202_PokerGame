#include <iostream>
#include "poker_engine.hpp"
#include "game_state.hpp"

#include "poker_engine_state.hpp"

PokerEngine::PokerEngine(PokerGame& poker_game)
    : game(poker_game) {
    block = new PokerEngineStatesBlock(game);
    auto [initial_state, _] = block->initial_state();
    state = initial_state;
}

PokerEngine::~PokerEngine() {
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

GameAction::Result PokerEngine::make_moves() {
        if (game.has_ended()) {
        return GameAction::OK;
    }
    GameState current_state = {};

    PlayerType current_turn = game.get_player_turn();

    if (current_turn == PlayerType::Human) {
        GameAction::Result result = make_move(PlayerType::Human, game.get_human_player().get_move(current_state));
        if (!result.ok || game.has_ended()) {
            return result;
        }
    }

    if (game.get_player_turn() == PlayerType::Computer) {
        current_state.community_cards = game.get_community_cards();
        current_state.hands = game.get_computer_player().hand;
        current_state.stage = state->enum_state_;
        current_state.current_bet = game.get_human_player().current_bet;

        Move computer_move = game.get_computer_player().get_move(current_state);
        game.set_player_move(PlayerType::Computer, computer_move);

        GameAction::Result result = make_move(PlayerType::Computer, computer_move);

        if (!result.ok || game.has_ended()) {
            std::cout<< "Game Ended!" << std::endl;
            return result;
        }
    }

    return GameAction::OK;
}

void PokerEngine::reset() {
    delete block;
    block = new PokerEngineStatesBlock(game);
    auto [initial_state, _] = block->initial_state();
    state = initial_state;
}
