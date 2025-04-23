#include <iostream>
#include "poker_engine.hpp"
#include "game_state.hpp"
#include <queue>
#include <iostream>

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

ControllerResult PokerEngine::make_moves() {
    
    ControllerResult response ;
    GameAction::Result engine_result;
    GameState current_state = {};
    
    int no_player_acted = 0;
    Move computer_move = {};
    
    while(game.get_playing_queue().size() != 0 ){

        Player* current_player = game.get_playing_queue().front();
        
        
        if(current_player->player_type == PlayerType::Human ){
            
            if(game.is_human_made_ui_choice() == true){
                engine_result = make_move(PlayerType::Human, game.get_human_player().get_move(current_state));
                
                if(game.has_ended()){
                    game.set_human_made_ui_choice(false);
                    return  ControllerResult{ResultState::GAME_ENDED}; 
                }
                if (!engine_result.ok) {
                    game.set_human_made_ui_choice(false);
                    break;
                }
                no_player_acted++;
            
            }else{
                
                std::cout << "Waiting for Human Choice"<< std::endl;
                //return GameAction::Result{true,"",GameAction::ResultState::WAIT_FOR_HUMAN_PLAYER}; 
                if(no_player_acted > 0){
                    std::cout << "Returning computer move" << std::endl;
                    return {ResultState::HUMAN_PLAYER_ACTION,computer_move};
                }else{
                    return {ResultState::HUMAN_PLAYER_ACTION};
                }
                
            }
        
        }else if(current_player->player_type == PlayerType::Computer){
            
            current_state.community_cards = game.get_community_cards();
            current_state.hands = game.get_computer_player().hand;
            current_state.stage = state->enum_state_;
            current_state.current_bet = game.get_human_player().current_bet;
            computer_move = game.get_computer_player().get_move(current_state);
            //computer_move = Fold{};
            game.set_player_move(PlayerType::Computer, computer_move);
            engine_result = make_move(PlayerType::Computer, computer_move);
            game.set_human_made_ui_choice(false);
            
            if(game.has_ended()){
                return  ControllerResult{ResultState::GAME_ENDED,computer_move};                
            }
            if (!engine_result.ok) {
                std::cout << " Unhandled Error " << std::endl;
                break;
            }
            no_player_acted++;
            
        }
        
    }
    
    if(!engine_result.ok){
        std::cout << "Action required " << engine_result.error_message.value_or("Unkown Error") << std::endl;
        return ControllerResult{ResultState::HUMAN_PLAYER_ACTION_ERROR,{},engine_result.error_message};
    }
    if(game.has_ended()){
       return  ControllerResult{ResultState::GAME_ENDED}; 
    }
    
    std::cout << " << Default case reached << " << std::endl;
    return  ControllerResult{ResultState::HUMAN_PLAYER_ACTION};
}
