#pragma once

#include "Wheel.hpp"

#include <optional>

class WheelManager {
public:
    std::optional<size_t> chooseIndex(const Wheel& wheel);
};
