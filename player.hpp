#ifndef PLAYER_H
#define PLAYER_H

#include <vector>
#include "card.hpp"

class Player {
public:
    Player();
    void addCard(const Card* card);
    void clearHand();
    std::vector<const Card*> getHand() const;
private:
    std::vector<const Card*> hand;
};

#endif // PLAYER_H
