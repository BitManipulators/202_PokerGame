#include "deck.hpp"
#include <algorithm>
#include <random>
#include <chrono>

Deck::Deck() {
    // Create a standard 52-card deck.
    for (int s = Hearts; s <= Spades; s++) {
        for (int r = 1; r <= 13; r++) {
            cards.push_back(Card(static_cast<Suit>(s), r));
        }
    }
}

void Deck::shuffle() {
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::shuffle(cards.begin(), cards.end(), std::default_random_engine(seed));
}

Card Deck::dealCard() {
    Card card = cards.back();
    cards.pop_back();
    return card;
}

bool Deck::isEmpty() const {
    return cards.empty();
}
