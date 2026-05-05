#pragma once

#include "WheelItem.hpp"
#include "WheelMode.hpp"

#include <string>
#include <vector>

class Wheel {
public:
    void addItem(std::string name);
    void clear();
    void reset();

    const std::vector<WheelItem>& items() const;

    void setMode(WheelMode mode);
    WheelMode mode() const;

private:
    std::vector<WheelItem> items_;
    WheelMode mode_ = WheelMode::Choice;
};
