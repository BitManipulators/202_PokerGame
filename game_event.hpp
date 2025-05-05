#pragma once

#include "move.hpp"
#include "player.hpp"

struct GameEvent {
    virtual ~GameEvent() = default;
};

struct MoveEvent : public GameEvent {
    PlayerType player;
    Move move;

    MoveEvent(PlayerType p, Move m) : player(p), move(std::move(m)) {}
};

struct StateTransitionEvent : public GameEvent {
    std::string from_state;
    std::string to_state;

    StateTransitionEvent(const std::string& from, const std::string& to)
        : from_state(from), to_state(to) {}
};
