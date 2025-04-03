#ifndef DECK_H
#define DECK_H

#include <vector>
#include "card.hpp"

class Deck {
public:
    Deck();
    void shuffle();
    Card dealCard();
    bool isEmpty() const;
private:
    std::vector<Card> cards;
};

#endif // DECK_H
