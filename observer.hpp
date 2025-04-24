#pragma once
#include "game_event.hpp"

class Observer {
public:
    virtual ~Observer() = default;
    virtual void onGameEvent(const GameEvent& event) = 0;
};
