#pragma once
#include <vector>
#include "card.hpp"

enum class PlayerType {
    Human,
    Computer
};

class Player {
public:
    Player(PlayerType player_type);

    void addCard(const Card* card);
    void clearHand();

    bool has_acted;
    std::size_t chips;
    std::size_t current_bet;
    std::vector<const Card*> hand;
    PlayerType player_type;
};
