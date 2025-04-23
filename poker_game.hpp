#pragma once

#include "deck.hpp"
#include "player.hpp"
#include "poker_hand_evaluator.hpp"
#include "observer.hpp"

#include <optional>
#include <vector>

namespace GameAction {

struct Result {
    bool ok;
    std::optional<std::string> error_message;
};

static Result OK = {
    .ok = true,
    .error_message = {},
};

inline Result ERROR(const std::string& error_message) {
    return {
        .ok = false,
        .error_message = error_message,
    };
}

} // GameActionResult

class PokerGame {
public:
    PokerGame();
    virtual ~PokerGame();

    GameAction::Result perform_call(PlayerType player_type);
    GameAction::Result perform_fold(PlayerType player_type);
    GameAction::Result perform_raise(PlayerType player_type, const std::size_t value);

    void deal_hole_cards();
    void deal_flop();
    void deal_turn();
    void deal_river();

    void clear_player_actions();
    bool all_players_have_acted();
    void set_player_move(PlayerType type, Move move);

    void rotate_dealer();
    void rotate_player_turn();
    void post_blinds();
    void shuffle_deck();
    void prepare_new_game();
    void determine_winner();

    const bool has_ended() const;
    const std::size_t get_pot() const;
    const Player& get_human_player() const;
    const Player& get_computer_player() const;
    const std::optional<PokerHandWinner> get_winner() const;
    const std::vector<const Card*>& get_community_cards() const;

    std::string get_winning_hand_description() const;
    PlayerType get_player_turn() const;
    void set_player_turn(PlayerType type) ;


    Player* get_player(PlayerType player_type);
    std::tuple<Player*, Player*> get_acting_and_other_player(PlayerType player_type);

    PokerEngineEnumState stage;

    void add_observer(Observer* observer);
    void notifyObservers();


private:
    std::size_t pot;
    std::size_t small_blind;
    std::size_t big_blind;

    Deck deck;
    Player* human_player;
    Player* computer_player;
    std::vector<const Card*> community_cards;

    PlayerType dealer;
    PlayerType player_turn;

    std::optional<PokerHandWinner> winner;
    std::optional<PokerHandEvaluation> hand_evaluation;

    std::vector<Observer*> observers;
};
