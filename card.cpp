#include "card.hpp"

namespace {

std::string get_card_name(const Suit suit, const Rank rank) {
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

std::string load_card_image(const Suit suit, const Rank rank) {
    return ":/images/" + get_card_name(suit, rank) + ".png";
}

} // namespace

std::map<std::pair<Suit, Rank>, std::shared_ptr<const Card>> Card::card_cache;

Card::Card(Suit suit, Rank rank)
    : suit(suit)
    , rank(rank) {
    image_path = load_card_image(suit, rank);
}

// Singleton function to get or create a card
std::shared_ptr<const Card> Card::get_card(Suit suit, Rank rank) {
    auto key = std::make_pair(suit, rank);
    auto it = card_cache.find(key);
    if (it == card_cache.end()) {
        card_cache[key] = std::shared_ptr<const Card>(new Card(suit, rank));
    }
    return card_cache[key];
}

Suit Card::get_suit() const {
    return suit;
}

Rank Card::get_rank() const {
    return rank;
}

int Card::get_value() const {
    return (int)rank;
}

const std::string& Card::get_card_image_path() const {
    return image_path;
}
