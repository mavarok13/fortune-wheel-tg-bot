#pragma once

#include "WheelItem.hpp"
#include "WheelMode.hpp"

#include <cstddef>
#include <string>
#include <vector>

class Wheel {
public:
    void addItem(std::string name);
    void removeItem(const std::string& name);
    void clear();
    void reset();

    const std::vector<WheelItem>& items() const;
    const WheelItem& itemAt(size_t index) const;
    bool containsItem(const std::string& name) const;
    void deactivateItem(size_t index);

    void setMode(WheelMode mode);
    WheelMode mode() const;

private:
    std::vector<WheelItem> items_;
    WheelMode mode_ = WheelMode::Choice;
};
