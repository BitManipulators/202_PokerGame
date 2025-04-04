#include "pokergame.hpp"

PokerGame::PokerGame() {
    startNewGame();
}

void PokerGame::startNewGame() {
    deck = Deck();
    deck.shuffle();
    player1.clearHand();
    player2.clearHand();
    communityCards.clear();
   // dealHoleCards();
    static bool dealerToggle = false;
    dealerToggle = !dealerToggle;
    setDealer(dealerToggle);

    dealHoleCards();
    setStage(GameStage::PreFlop);
}

void PokerGame::dealHoleCards() {
    // Deal 2 cards to each player.
    player1.addCard(deck.dealCard());
    player2.addCard(deck.dealCard());
    player1.addCard(deck.dealCard());
    player2.addCard(deck.dealCard());
}

void PokerGame::dealFlop() {
    // Burn one card.
    deck.dealCard();
    // Deal 3 community cards.
    for (int i = 0; i < 3; i++) {
        communityCards.push_back(deck.dealCard());
    }
}

void PokerGame::dealTurn() {
    // Burn one card.
    deck.dealCard();
    // Deal one community card.
    communityCards.push_back(deck.dealCard());
}

void PokerGame::dealRiver() {
    // Burn one card.
    deck.dealCard();
    // Deal one community card.
    communityCards.push_back(deck.dealCard());
}

const std::vector<Card>& PokerGame::getCommunityCards() const {
    return communityCards;
}

Player& PokerGame::getPlayer1() {
    return player1;
}

Player& PokerGame::getPlayer2() {
    return player2;
}
