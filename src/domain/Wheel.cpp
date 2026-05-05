#include "Wheel.hpp"

#include <utility>

void Wheel::addItem(std::string name) {
    WheelItem item;
    item.id = std::to_string(items_.size() + 1);
    item.name = std::move(name);
    item.active = true;
    items_.push_back(std::move(item));
}

void Wheel::clear() { items_.clear(); }

void Wheel::reset() {
    for (auto& item : items_) {
        item.active = true;
    }
}

const std::vector<WheelItem>& Wheel::items() const { return items_; }

void Wheel::setMode(WheelMode mode) { mode_ = mode; }

WheelMode Wheel::mode() const { return mode_; }
