#pragma once
#include "card.hpp"
#include "game_constants.hpp"

#include <vector>

struct GameState{
       
       std::vector<const Card*> hands;
       std::vector<const Card*> community_cards;
       PokerEngineEnumState stage;
       std::size_t current_bet;
       std::size_t pot_size;
       std::size_t computer_chips;

};