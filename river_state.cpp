#include "river_state.hpp"
#include "pokergame.hpp"
#include "showdown_state.hpp"

void RiverState::handle(PokerGame& game) {
    game.setState(new ShowDownState()); // Transition to next state
}
