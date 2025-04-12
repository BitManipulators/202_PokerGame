#ifndef DECK_H
#define DECK_H

#include <vector>
#include "card.hpp"

class Deck {
public:
    Deck();
    void shuffle();
    const Card* dealCard();
    bool isEmpty() const;
private:
    std::vector<std::shared_ptr<const Card>> cards;
};

#endif // DECK_H
