#include "game_controller.hpp"

UIComponents GameController::get_ui_components() const
{
    const Player &computer_player = game.get_computer_player();
    const Player &human_player = game.get_human_player();
    const std::size_t c_bet = (human_player.current_bet > computer_player.current_bet)
                                        ? human_player.current_bet
                                        : computer_player.current_bet;
    
    UIComponents ui_objects = {};
    ui_objects.community_cards = game.get_community_cards();
    ui_objects.human_player_cards = human_player.hand;
    ui_objects.computer_player_cards = computer_player.hand;
    ui_objects.current_bet = c_bet;
    ui_objects.pot_value = game.get_pot();
    ui_objects.human_chips =  human_player.chips;
    ui_objects.computer_chips = computer_player.chips;
    ui_objects.is_game_ended = game.has_ended();
    
    
    if(game.has_ended() && game.get_winner().has_value()){
        
        ui_objects.winner = game.get_winner().value(); 
        ui_objects.winning_hand_desc = game.get_winning_hand_description();
    }
    
    return ui_objects;
}

Move GameController::get_computer_move() const
{
    GameState current_state = {};
    current_state.community_cards = game.get_community_cards();
    current_state.hands = game.get_computer_player().hand;
    current_state.stage = engine.state->enum_state_;
    current_state.current_bet = game.get_human_player().current_bet;
    Move computer_move = game.get_computer_player().get_move(current_state);
    return computer_move;
}

void GameController::set_human_made_move()
{
    human_player_made_ui_choice = true;
}

Move GameController::get_human_move() const
{
    GameState current_state = {};
    return game.get_human_player().get_move(current_state);
}

void GameController::set_human_move(Move move){
    
    game.set_player_move(PlayerType::Human, move);
}

UIState GameController::new_game()
{
        engine.new_game();
        human_player_made_ui_choice = false;
        return UIState::HUMAN_PLAYER_ACTION;
        
}

UIState GameController::next_move()
{
    
    GameControllerResult response;
    GameAction::Result engine_result;

    int player_acted = 0;
    UIComponents ui_objects;

    while (game.get_playing_queue().size() != 0)
    {

        Player *current_player = game.get_playing_queue().front();
        ui_objects = get_ui_components();
        Move computer_move;

        if (current_player->player_type == PlayerType::Human)
        {
            if (player_acted > 0){
                    // set computer move in response object
                    std::cout << "Setting computer move" << std::endl;
                    ui_objects.computer_move = computer_move;
            }        
            
            if (human_player_made_ui_choice){
                engine_result = engine.make_move(PlayerType::Human, get_human_move());
                if (!engine_result.ok){
                    human_player_made_ui_choice = false;
                    break;
                }
                player_acted++;
            }
            else{
                std::cout << "Waiting for Human Choice" << std::endl;
                return UIState::HUMAN_PLAYER_ACTION;
            }
        }
        
        else if (current_player->player_type == PlayerType::Computer){

            Move computer_move = get_computer_move();
            game.set_player_move(PlayerType::Computer, computer_move);
            ui_objects.computer_move = computer_move;
            engine_result = engine.make_move(PlayerType::Computer, computer_move);
            if (!engine_result.ok){   
                std::cout << " Unhandled Computer Move Error " << std::endl;
                break;
            }
            human_player_made_ui_choice = false;
            player_acted++;
        }
        
        if (game.has_ended()){
            break;
        }
    }

    if (!engine_result.ok)
    {
        ui_objects.error_message = engine_result.error_message.value_or("Unkown Error");
        std::cout << "Action required " << engine_result.error_message.value_or("Unkown Error") << std::endl;
        return UIState::HUMAN_PLAYER_ACTION_ERROR;
    }
    if (game.has_ended())
    {
        return UIState::GAME_ENDED;
    }

    std::cout << " << Default case reached << " << std::endl;
    return UIState::HUMAN_PLAYER_ACTION_UNKNOWN;
}