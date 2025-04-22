#include "player.hpp"

Player::Player(PlayerType type)
    : chips(1000)
    , current_bet(0)
    , has_acted(false)
    , player_type(type) {}

void Player::add_card(const Card* card) {
    hand.push_back(card);
}

void Player::clear_hand() {
    hand.clear();
}

void Player::set_move(Move move) {
    latest_move = move;
}

Move HumanPlayer::get_move(GameState current_state) const {
    return latest_move;
}

Move ComputerPlayer::get_move(GameState current_state) const {
    return strategy->getNextMove(current_state);
}
