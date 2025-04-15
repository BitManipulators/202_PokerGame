#ifndef POKERGAME_H
#define POKERGAME_H

#include "deck.hpp"
#include "player.hpp"
#include "poker_game_state.hpp"

#include <vector>
#include <QMessageBox>

enum class GameStage {
    PreFlop,        // Hole cards dealt
    Flop,           // Flop is dealt
    SecondBetting,  // Betting round (dealer acts first)
    Turn,           // Turn card dealt
    ThirdBetting,   // Betting round
    River,          // River card dealt
    FinalBetting,   // Final betting round
    Showdown        // Players reveal hands; winner determined
};


class PokerGame {
private:
    PokerGameState* currentState;
    Deck deck;
    Player player1;
    Player player2;
    std::vector<const Card*> communityCards;
    GameStage currentStage;
    bool player1Dealer;

public:
    PokerGame();
    ~PokerGame();
    void startNewGame();
    void dealHoleCards();
    void dealFlop();
    void dealTurn();
    void dealRiver();
    const std::vector<const Card*>& getCommunityCards() const;
    Player& getPlayer1();
    Player& getPlayer2();
    int determineWinner();
    // Methods for game stage
    void setStage(GameStage stage) { currentStage = stage; }
    GameStage getStage() const { return currentStage; }

    // Dealer information
    void setDealer(bool isPlayer1Dealer) { player1Dealer = isPlayer1Dealer; }
    bool isPlayer1Dealer() const { return player1Dealer; }

    // State management
    void setState(PokerGameState* newState);
    void handleState(); // delegate to current state's handle()
    const char* getStateName() const;
    QString getStateMessage() const;
};

#endif // POKERGAME_H
