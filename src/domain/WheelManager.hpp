#pragma once

#include "WheelItem.hpp"
#include "Wheel.hpp"

#include <optional>

class WheelManager {
public:
    std::optional<size_t> chooseIndex();

    // TODO: realize this methods
    // void setCurrentWheel(Wheel wheel);
    // void addItem(WheelItem item);
    // void editItem(WheelItem item);
    // void removeItem(WheelItem item);
private:
    Wheel * currentWheel_ = nullptr;
};
