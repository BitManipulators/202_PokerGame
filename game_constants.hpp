#pragma once

enum class Difficulty {
    Easy = 0,Medium,Hard
};

enum class PokerEngineEnumState {
    PreFlop,
    Flop,
    Turn,
    River,
    Showdown,
    Folded
};
