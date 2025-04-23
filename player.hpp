#pragma once

#include "card.hpp"
#include "move.hpp"
#include "computer_strategy.hpp"
#include "game_constants.hpp"
#include "game_state.hpp"

#include <vector>
#include <iostream>

enum class PlayerType {
    Human,
    Computer
};

std::ostream& operator<<(std::ostream& os, PlayerType const& player_type);


class Player {
public:
    Player(PlayerType player_type);
    virtual ~Player() = default;

    virtual void set_move(Move move);
    virtual Move get_move(GameState current_state) const = 0;

    void add_card(const Card* card);
    void clear_hand();

    bool has_acted;
    std::size_t chips;
    std::size_t current_bet;
    std::vector<const Card*> hand;
    PlayerType player_type;

    const std::variant<Fold, Call, Raise>& getLatestMove() const {
        return latest_move;
    }

protected:
    Move latest_move;
};

class HumanPlayer : public Player {
public:
    HumanPlayer() : Player(PlayerType::Human) {}
    virtual ~HumanPlayer() = default;

    virtual Move get_move(GameState current_state) const override;
};

class ComputerPlayer : public Player {
public:
    ComputerPlayer(Difficulty d) : Player(PlayerType::Computer) {
        switch(d){
            case Difficulty::Easy:
                strategy = std::make_unique<EasyStrategy>();
                break;
            case Difficulty::Medium:
                strategy = std::make_unique<MediumStrategy>();
                break;

            default :
                throw std::runtime_error("Unexpected Input");
        }


    }
    virtual ~ComputerPlayer() = default;

    virtual Move get_move(GameState current_state) const override;
private:
    std::unique_ptr<ComputerStrategy> strategy;
};
