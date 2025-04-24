#pragma once
#include "observer.hpp"
#include "move.hpp"
#include "game_event.hpp"
#include <iostream>
#include <variant>

class ConsoleLogger : public Observer {
public:
    void onGameEvent(const GameEvent& event) override {
        if (const auto* moveEvent = dynamic_cast<const MoveEvent*>(&event)) {
            std::string player = (moveEvent->player == PlayerType::Human) ? "You" : "Computer";

            if (std::holds_alternative<Call>(moveEvent->move)) {
                std::cout << player << " called.!!!!!" << std::endl;
            } else if (std::holds_alternative<Fold>(moveEvent->move)) {
                std::cout << player << " folded.!!!!!!!" << std::endl;
            } else if (std::holds_alternative<Raise>(moveEvent->move)) {
                std::cout << player << " raised to "
                          << std::get<Raise>(moveEvent->move).amount << " chips!!!!!!." << std::endl;
            }
        }

    else if (const auto* transitionEvent = dynamic_cast<const StateTransitionEvent*>(&event)) {
        std::cout << "------------------Transitioning from " << transitionEvent->from_state
                  << " to " << transitionEvent->to_state << std::endl;
    }
    }
};
