#pragma once

#include <QString>
class PokerGame; // Forward declaration

class PokerGameState {
public:
    virtual ~PokerGameState() = default;
    virtual void handle(PokerGame& game) = 0;
    virtual const char* name() const = 0;
    virtual QString getMessage() const { return QString(); }
};
