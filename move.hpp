#pragma once

#include <variant>

struct Fold {};
struct Call {};
struct Raise {
    std::size_t amount;
};

using Move = std::variant<Fold, Call, Raise>;
