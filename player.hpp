#pragma once

#include "card.hpp"
#include "move.hpp"

#include <vector>

enum class PlayerType {
    Human,
    Computer
};

class Player {
public:
    Player(PlayerType player_type);
    virtual ~Player() = default;

    virtual void set_move(Move move);
    virtual Move get_move() const = 0;

    void add_card(const Card* card);
    void clear_hand();

    bool has_acted;
    std::size_t chips;
    std::size_t current_bet;
    std::vector<const Card*> hand;
    PlayerType player_type;

protected:
    Move latest_move;
};

class HumanPlayer : public Player {
public:
    HumanPlayer() : Player(PlayerType::Human) {}
    virtual ~HumanPlayer() = default;

    virtual Move get_move() const override;
};

class ComputerPlayer : public Player {
public:
    ComputerPlayer() : Player(PlayerType::Computer) {}
    virtual ~ComputerPlayer() = default;

    virtual Move get_move() const override;
};
