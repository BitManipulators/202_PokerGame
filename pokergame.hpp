#ifndef POKERGAME_H
#define POKERGAME_H

#include "deck.hpp"
#include "player.hpp"
#include <vector>

class PokerGame {
public:
    PokerGame();
    void startNewGame();
    void dealHoleCards();
    void dealFlop();
    void dealTurn();
    void dealRiver();
    const std::vector<Card>& getCommunityCards() const;
    Player& getPlayer1();
    Player& getPlayer2();
    int determineWinner();
private:
    Deck deck;
    Player player1;
    Player player2;
    std::vector<Card> communityCards;
};

#endif // POKERGAME_H
