#include "preflop_state.hpp"
#include "pokergame.hpp"
#include "flop_state.hpp"

void PreFlopState::handle(PokerGame& game) {
    game.dealFlop();
    game.setState(new FlopState()); // Transition to next state
}
