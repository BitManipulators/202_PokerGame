#include "deck.hpp"
#include <algorithm>
#include <random>
#include <chrono>

Deck::Deck() {
    // Create a standard 52-card deck.
    for (Suit suit : suits) {
        for (Rank rank : ranks) {
            cards.push_back(Card::getCard(suit, rank));
        }
    }
}

void Deck::shuffle() {
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::shuffle(cards.begin(), cards.end(), std::default_random_engine(seed));
}

// suriya- comment Why shared_ptr to normal ptr 
const Card* Deck::dealCard() {
    std::shared_ptr<const Card> card = cards.back();
    cards.pop_back();
    return card.get();
}

bool Deck::isEmpty() const {
    return cards.empty();
}
