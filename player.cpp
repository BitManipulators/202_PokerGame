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

ComputerPlayer::ComputerPlayer(Difficulty d) : Player(PlayerType::Computer) {
    switch(d) {
    case Difficulty::Easy:
        strategy = std::make_unique<EasyStrategy>();
        break;
    case Difficulty::Medium:
        strategy = std::make_unique<MediumStrategy>();
        break;
    // case Difficulty::Hard:
    //     strategy = std::make_unique<HardStrategy>();
    //     break;

    default:
        throw std::runtime_error("Unexpected Input");
    }
}

Move ComputerPlayer::get_move(GameState current_state) const {
    return strategy->get_next_move(current_state);
}

void ComputerPlayer::set_strategy(std::unique_ptr<ComputerStrategy> new_strategy) {
    strategy = std::move(new_strategy);
}
