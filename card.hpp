#pragma once

#include <array>
#include <map>
#include <memory>
#include <string>

enum class Suit {
    Hearts, Diamonds, Clubs, Spades
};

enum class Rank {
    Two = 2, Three, Four, Five, Six, Seven, Eight, Nine, Ten, Jack, Queen, King, Ace
};

constexpr std::array<Suit, 4> suits { Suit::Hearts, Suit::Diamonds, Suit::Clubs, Suit::Spades };

constexpr std::array<Rank, 13> ranks = {
    Rank::Ace, Rank::Two, Rank::Three, Rank::Four, Rank::Five, Rank::Six,
    Rank::Seven, Rank::Eight, Rank::Nine, Rank::Ten, Rank::Jack, Rank::Queen, Rank::King
};

class Card {
public:
    // Singleton get or create card
    static std::shared_ptr<const Card> getCard(Suit suit, Rank rank);

    Suit getSuit() const;
    Rank getRank() const;
    int getValue() const;
    const std::string& getCardImagePath() const;

private:
    // Private constructor to ensure singleton
    Card(Suit suit, Rank rank);

    // Prevent copying and moving to ensure singleton
    Card(const Card&) = delete;
    Card& operator=(const Card&) = delete;
    Card(Card&&) = delete;
    Card& operator=(Card&&) = delete;

    Suit suit;
    Rank rank;
    std::string image_path;

    static std::map<std::pair<Suit, Rank>, std::shared_ptr<const Card>> card_cache;
};
