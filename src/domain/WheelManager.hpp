#pragma once

#include "Wheel.hpp"
#include "WheelItem.hpp"

#include <optional>
#include <string>

class WheelManager {
public:
    void setCurrentWheel(Wheel& wheel);
    void clearCurrentWheel();
    bool hasCurrentWheel() const;
    Wheel& currentWheel();
    const Wheel& currentWheel() const;

    void addItem(std::string name);
    void removeItem(const std::string& name);
    void reset();
    void clear();
    void setMode(WheelMode mode);

    std::optional<size_t> chooseIndex();

private:
    Wheel* currentWheel_ = nullptr;
};
