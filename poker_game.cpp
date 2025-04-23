#include "poker_game.hpp"
#include "game_constants.hpp"

#include "poker_hand_evaluator.hpp"
#include <iostream>

PokerGame::PokerGame()
    : pot(0)
    , small_blind(5)
    , big_blind(10)
    , human_player(new HumanPlayer())
    , computer_player(new ComputerPlayer(Difficulty::Easy))
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
        std::cerr << "Invalid PlayerType!" << std::endl;
        // Optionally throw an exception or return nullptrs
        acting_player = nullptr;
        other_player = nullptr;
        break;
    }

    return {acting_player, other_player};
}

GameAction::Result PokerGame::perform_call(PlayerType player_type) {
   
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

    std::cout  << calling_player->player_type << "  player chips after call  " << calling_player->chips << std::endl;
    std::cout  << other_player->player_type << "  player chips after call  " << other_player->chips << std::endl;
    
    next_player_turn();
    
    return GameAction::OK;
}

GameAction::Result PokerGame::perform_fold(PlayerType player_type) {
    
    auto [folding_player, other_player] = get_acting_and_other_player(player_type);

    other_player->chips += pot;
    winner = (other_player->player_type == PlayerType::Human) ? PokerHandWinner::Player1 : PokerHandWinner::Player2;

    return GameAction::OK;
}

GameAction::Result PokerGame::perform_raise(PlayerType player_type, const std::size_t raise) {
    
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
    

    pot += raise;

    std::cout << "amount to raised " << raise << std::endl;

    std::cout  << raising_player->player_type << "  player chips after raise  " << raising_player->chips << std::endl;
    std::cout  << other_player->player_type << "  player chips after raise  " << other_player->chips << std::endl;
    
    next_player_turn();
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

void PokerGame::next_player_turn() {
    playing_turn_queue.pop();
}

PlayerType PokerGame::get_dealer() const{
    return dealer;
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
    human_player->add_card(deck.dealCard());
    computer_player->add_card(deck.dealCard());
    human_player->add_card(deck.dealCard());
    computer_player->add_card(deck.dealCard());
}

void PokerGame::deal_flop() {
    // Burn one card.
    deck.dealCard();

    // Deal 3 community cards.
    for (int i = 0; i < 3; i++) {
        community_cards.push_back(deck.dealCard());
    }
}

void PokerGame::deal_turn() {
    // Burn one card.
    deck.dealCard();

    // Deal one community card.
    community_cards.push_back(deck.dealCard());
}

void PokerGame::deal_river() {
    // Burn one card.
    deck.dealCard();

    // Deal one community card.
    community_cards.push_back(deck.dealCard());
}

void PokerGame::clear_player_actions() {
    human_player->has_acted = false;
    computer_player->has_acted = false;    
}

void PokerGame::set_playing_order() {
    playing_turn_queue.push(human_player);
    playing_turn_queue.push(computer_player);
}


bool PokerGame::all_players_have_acted() {
    //return human_player->has_acted && computer_player->has_acted;
    return playing_turn_queue.size() == 0;
}

const bool PokerGame::has_ended() const {
    return winner.has_value();
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
    hand_evaluation = {};
}

std::queue<Player*> PokerGame::get_playing_queue() const{
    return playing_turn_queue;
}

bool PokerGame::is_human_made_ui_choice() const{
     return human_player_made_ui_choice;
}
void PokerGame::set_human_made_ui_choice(bool flag) {
    human_player_made_ui_choice = flag;
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
}

std::string PokerGame::get_winning_hand_description() const {
    if (!winner.has_value() || !hand_evaluation.has_value()) {
        return "Unknown hand";
    }
    return hand_evaluation.value().to_string();
}