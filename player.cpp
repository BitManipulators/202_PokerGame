#include "player.hpp"

Player::Player(PlayerType type)
    : chips(1000)
    , current_bet(0)
    , has_acted(false)
    , player_type(type) {}

void Player::addCard(const Card* card) {
    hand.push_back(card);
}

void Player::clearHand() {
    hand.clear();
}
