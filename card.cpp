#include "card.hpp"

namespace {

std::string getCardName(const Suit suit, const Rank rank) {
    std::string suitStr;
    switch(suit) {
    case Suit::Hearts:   suitStr = "hearts"; break;
    case Suit::Diamonds: suitStr = "diamonds"; break;
    case Suit::Clubs:    suitStr = "clubs"; break;
    case Suit::Spades:   suitStr = "spades"; break;
    }

    std::string rankStr;
    if(rank == Rank::Ace) rankStr = "ace";
    else if(rank == Rank::Jack) rankStr = "jack";
    else if(rank == Rank::Queen) rankStr = "queen";
    else if(rank == Rank::King) rankStr = "king";
    else rankStr = std::to_string((int)rank);

    return rankStr + "_of_" + suitStr;
}

std::string loadCardImage(const Suit suit, const Rank rank) {
    return ":/images/" + getCardName(suit, rank) + ".png";
}

} // namespace

std::map<std::pair<Suit, Rank>, std::shared_ptr<const Card>> Card::card_cache;

Card::Card(Suit suit, Rank rank)
    : suit(suit)
    , rank(rank) {
    image_path = loadCardImage(suit, rank);
}

// Singleton function to get or create a card
std::shared_ptr<const Card> Card::getCard(Suit suit, Rank rank) {
    auto key = std::make_pair(suit, rank);
    auto it = card_cache.find(key);
    if (it == card_cache.end()) {
        card_cache[key] = std::shared_ptr<const Card>(new Card(suit, rank));
    }
    return card_cache[key];
}

Suit Card::getSuit() const {
    return suit;
}

Rank Card::getRank() const {
    return rank;
}

int Card::getValue() const {
    return (int)rank;
}

const std::string& Card::getCardImagePath() const {
    return image_path;
}
