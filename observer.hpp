#pragma once

#include "game_event.hpp"

class Observer {
public:
    virtual ~Observer() = default;
    virtual void on_game_event(const GameEvent& event) = 0;
};
