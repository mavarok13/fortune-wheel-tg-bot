#pragma once

#include "Wheel.hpp"
#include "WheelItem.hpp"

#include <cstdint>
#include <optional>
#include <string>

class WheelManager {
public:
    void setCurrentWheel(Wheel& wheel);
    void clearCurrentWheel();
    bool hasCurrentWheel() const;
    Wheel& currentWheel();
    const Wheel& currentWheel() const;

    void addItem(std::string name, std::uint64_t score = 1);
    void setItemScore(const std::string& name, std::uint64_t score);
    void addItemScore(const std::string& name, std::uint64_t scoreDelta);
    void removeItem(const std::string& name);
    void reset();
    void clear();
    void setMode(WheelMode mode);

    std::optional<size_t> chooseIndex();

private:
    Wheel* currentWheel_ = nullptr;
};
