#include "flop_state.hpp"
#include "pokergame.hpp"
#include "turn_state.hpp"

void FlopState::handle(PokerGame& game) {
    game.dealTurn();
    game.setState(new TurnState()); // Transition to next state
}
