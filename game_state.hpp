#pragma once
#include "card.hpp"
#include "game_constants.hpp"

#include <vector>

struct GameState{
       
       std::vector<const Card*> hands;
       std::vector<const Card*> community_cards;
       PokerEngineEnumState stage;
       std::size_t current_bet;

};