#include "pokergame.hpp"

#include "poker_hand_evaluator.hpp"
#include "preflop_state.hpp"

PokerGame::PokerGame(): currentState(nullptr){
    startNewGame();
}

PokerGame::~PokerGame() {
    delete currentState;
}

void PokerGame::setState(PokerGameState* newState) {
    if (currentState) delete currentState;
    currentState = newState;
}

void PokerGame::handleState() {
    if (currentState) currentState->handle(*this);
}

const char* PokerGame::getStateName() const {
    return currentState ? currentState->name() : "Unknown";
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
    setState(new PreFlopState());
   // setStage(GameStage::PreFlop);
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

const std::vector<const Card*>& PokerGame::getCommunityCards() const {
    return communityCards;
}

Player& PokerGame::getPlayer1() {
    return player1;
}

Player& PokerGame::getPlayer2() {
    return player2;
}

int PokerGame::determineWinner() {
    PokerHandResult poker_hand_result = PokerHandEvaluator::determine_winner(
        player1.getHand(), player2.getHand(), communityCards);

    switch(poker_hand_result.winner) {
        case PokerHandWinner::Player1:
            return 1;
        case PokerHandWinner::Player2:
            return 2;
        case PokerHandWinner::Tie:
        default:
            return 0;
    }
}

QString PokerGame::getStateMessage() const {
    return currentState ? currentState->getMessage() : "";
}
