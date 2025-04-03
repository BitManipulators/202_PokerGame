#include "player.hpp"

Player::Player() {}

void Player::addCard(const Card &card) {
    hand.push_back(card);
}

void Player::clearHand() {
    hand.clear();
}

std::vector<Card> Player::getHand() const {
    return hand;
}
