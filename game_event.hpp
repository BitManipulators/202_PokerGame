#pragma once
#include "player.hpp"
#include "move.hpp"

struct GameEvent {
    virtual ~GameEvent() = default;
};

struct MoveEvent : public GameEvent {
    PlayerType player;
    Move move;

    MoveEvent(PlayerType p, Move m) : player(p), move(std::move(m)) {}
};
