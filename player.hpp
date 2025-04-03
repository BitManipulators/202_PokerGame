#ifndef PLAYER_H
#define PLAYER_H

#include <vector>
#include "card.hpp"

class Player {
public:
    Player();
    void addCard(const Card &card);
    void clearHand();
    std::vector<Card> getHand() const;
private:
    std::vector<Card> hand;
};

#endif // PLAYER_H
