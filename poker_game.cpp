#include "poker_game.hpp"

#include "game_constants.hpp"
#include "poker_hand_evaluator.hpp"

#include <iostream>
#include <optional>


PokerGame::PokerGame()
    : pot(0)
    , small_blind(5)
    , big_blind(10)
    , human_player(new HumanPlayer())
    , computer_player(new ComputerPlayer(Difficulty::Medium))
    , player_turn(PlayerType::Human)
    , dealer(PlayerType::Human) {}

PokerGame::~PokerGame() {
    delete human_player;
    delete computer_player;
}

Player* PokerGame::get_player(PlayerType player_type) {
    switch(player_type) {
    case PlayerType::Human:
        return human_player;
    case PlayerType::Computer:
    default:
        return computer_player;
    }
}


std::tuple<Player*, Player*> PokerGame::get_acting_and_other_player(PlayerType player_type) {
    Player* acting_player = nullptr; // Initialize to nullptr
    Player* other_player = nullptr;  // Initialize to nullptr

    switch(player_type) {
    case PlayerType::Human:
        acting_player = human_player;
        other_player = computer_player;
        break;
    case PlayerType::Computer:
        acting_player = computer_player;
        other_player = human_player;
        break;
    default:
        // Handle the case where player_type is unexpected
        // std::cerr << "Invalid PlayerType!" << std::endl;
        // Optionally throw an exception or return nullptrs
        acting_player = nullptr;
        other_player = nullptr;
        break;
    }

    return {acting_player, other_player};
}


GameAction::Result PokerGame::perform_call(PlayerType player_type) {
    if (player_type != player_turn) {
        return GameAction::ERROR("Wrong players turn!");
    }

    auto [calling_player, other_player] = get_acting_and_other_player(player_type);

    std::size_t amount_to_call = other_player->current_bet - calling_player->current_bet;
    if (calling_player->chips < amount_to_call) {
        // Call all-in
        amount_to_call = calling_player->chips;
    }

    calling_player->chips -= amount_to_call;
    calling_player->current_bet = other_player->current_bet;
    calling_player->has_acted = true;

    pot += amount_to_call;

    rotate_player_turn();

    return GameAction::OK;
}

GameAction::Result PokerGame::perform_fold(PlayerType player_type) {
    if (player_type != player_turn) {
        return GameAction::ERROR("Wrong players turn!");
    }

    auto [folding_player, other_player] = get_acting_and_other_player(player_type);

    other_player->chips += pot;
    winner = (other_player->player_type == PlayerType::Human) ? PokerHandWinner::Player1 : PokerHandWinner::Player2;
    return GameAction::OK;
}

GameAction::Result PokerGame::perform_raise(PlayerType player_type, const std::size_t raise) {
    if (player_type != player_turn) {
        return GameAction::ERROR("Wrong players turn!");
    }

    auto [raising_player, other_player] = get_acting_and_other_player(player_type);

    if (raise < (2 * other_player->current_bet)) {
        return GameAction::ERROR("Raise must be at least 2x other player's bet!");
    }

    std::size_t amount_raised = raise - raising_player->current_bet;
    if (amount_raised > raising_player->chips) {
        return GameAction::ERROR("Raise higher than player's chips!");
    }

    raising_player->chips -= amount_raised;
    raising_player->current_bet = raise;
    raising_player->has_acted = true;
    other_player->has_acted = false;

    pot += amount_raised;

    rotate_player_turn();

    return GameAction::OK;
}

void PokerGame::rotate_dealer() {
    switch(dealer) {
    case PlayerType::Human:
        dealer = PlayerType::Computer;
        break;
    case PlayerType::Computer:
        dealer = PlayerType::Human;
        break;
    }
}

void PokerGame::rotate_player_turn() {
    switch(player_turn) {
    case PlayerType::Human:
        player_turn = PlayerType::Computer;
        break;
    case PlayerType::Computer:
        player_turn = PlayerType::Human;
        break;
    }
}

PlayerType PokerGame::get_player_turn() const {
    return player_turn;
}

void PokerGame::set_player_turn(PlayerType type) {
     player_turn = type;
}

void PokerGame::shuffle_deck() {
    human_player->clear_hand();
    computer_player->clear_hand();
    community_cards.clear();

    deck = Deck();
    deck.shuffle();
}

void PokerGame::deal_hole_cards() {
    // Deal 2 cards to each player.
    human_player->add_card(deck.deal_card());
    computer_player->add_card(deck.deal_card());
    human_player->add_card(deck.deal_card());
    computer_player->add_card(deck.deal_card());
}

void PokerGame::deal_flop() {
    // Burn one card.
    deck.deal_card();

    // Deal 3 community cards.
    for (int i = 0; i < 3; i++) {
        community_cards.push_back(deck.deal_card());
    }
}

void PokerGame::deal_turn() {
    // Burn one card.
    deck.deal_card();

    // Deal one community card.
    community_cards.push_back(deck.deal_card());
}

void PokerGame::deal_river() {
    // Burn one card.
    deck.deal_card();

    // Deal one community card.
    community_cards.push_back(deck.deal_card());
}

void PokerGame::clear_player_actions() {
    human_player->has_acted = false;
    computer_player->has_acted = false;
}

bool PokerGame::all_players_have_acted() {
    return human_player->has_acted && computer_player->has_acted;
}

const bool PokerGame::has_ended() const {
    return winner.has_value();
}

const std::optional<PokerHand> PokerGame::get_winning_hand() const {
    return winning_hand;
}

const std::optional<PokerHandWinner> PokerGame::get_winner() const {
    return winner;
}

const std::size_t PokerGame::get_pot() const {
    return pot;
}

const std::vector<const Card*>& PokerGame::get_community_cards() const {
    return community_cards;
}

const Player& PokerGame::get_human_player() const {
    return *human_player;
}

const Player& PokerGame::get_computer_player() const {
    return *computer_player;
}

void PokerGame::determine_winner() {
    PokerHandResult poker_hand_result = PokerHandEvaluator::determine_winner(human_player->hand, computer_player->hand, community_cards);
    winner = poker_hand_result.winner;
    winning_hand = poker_hand_result.hand;
    hand_evaluation = poker_hand_result.evaluation;

    switch(winner.value()) {
    case PokerHandWinner::Tie:
        human_player->chips += pot / 2;
        computer_player->chips += pot / 2;
        break;
    case PokerHandWinner::Player1:
        human_player->chips += pot;
        break;
    case PokerHandWinner::Player2:
        computer_player->chips += pot;
        break;
    }
}

void PokerGame::prepare_new_game() {
    pot = 0;
    human_player->current_bet = 0;
    computer_player->current_bet = 0;
    winner = {};
    winning_hand = {};
    hand_evaluation = {};

    // Rotate dealer
    rotate_dealer();

    // Set player turn to Small Blind
    if (dealer == PlayerType::Human) {
        player_turn = PlayerType::Computer; // Human is dealer, so computer starts
    } else {
        player_turn = PlayerType::Human;    // Computer is dealer, so human starts
    }

    // Clear any previous moves/actions
    clear_player_actions();
}

void PokerGame::post_blinds() {
    Player* big_blind_player;
    Player* small_blind_player;
    if (dealer == PlayerType::Human) {
        big_blind_player = human_player;
        small_blind_player = computer_player;
    } else {
        big_blind_player = computer_player;
        small_blind_player = human_player;
    }

    std::size_t big_blind_amount = std::min(big_blind, big_blind_player->chips);
    std::size_t small_blind_amount = std::min(small_blind, small_blind_player->chips);

    big_blind_player->chips -= big_blind_amount;
    small_blind_player->chips -= small_blind_amount;

    pot += big_blind_amount;
    pot += small_blind_amount;
}

void PokerGame::set_player_move(PlayerType player_type, Move move) {
    get_player(player_type)->set_move(move);
    notify_game_event(std::make_shared<MoveEvent>(player_type, move));
}

std::string PokerGame::get_winning_hand_description() const {
    if (!winner.has_value() || !hand_evaluation.has_value()) {
        return "Unknown hand";
    }
    return hand_evaluation.value().to_string();
}

void PokerGame::add_observer(Observer* observer) {
    observers.push_back(observer);
}

void PokerGame::notify_game_event(std::shared_ptr<GameEvent> event) {
    std::cout << "[DEBUG] Notifying observers of game event " << observers.size() << " observers" << std::endl;
    for (auto* obs : observers) {
        std::cout << "[DEBUG] Notifying observer" << std::endl;
        obs->on_game_event(*event);
    }
}

void PokerGame::reset_game() {
    pot = 0;
    winner = {};
    hand_evaluation = {};

    human_player->reset();
    computer_player->reset();

    player_turn = PlayerType::Human;
    dealer = PlayerType::Human;

    deck = Deck();
    community_cards.clear();
}
