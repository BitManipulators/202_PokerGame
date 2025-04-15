#include "turn_state.hpp"
#include "pokergame.hpp"
#include "river_state.hpp"

void TurnState::handle(PokerGame& game) {
    game.dealRiver();
    game.setState(new RiverState()); // Transition to next state
}
