#include "Wheel.hpp"

#include "DomainExceptions.hpp"

#include <algorithm>
#include <utility>

void Wheel::setName(const std::string & name) {
    name_ = name;
}

void Wheel::addItem(const std::string & name) {
    WheelItem item;
    item.id = std::to_string(items_.size() + 1);
    item.name = std::move(name);
    item.active = true;
    items_.push_back(std::move(item));
}

void Wheel::renameItem(const std::string& oldName, const std::string& newName) {
    const auto itemIt = std::find_if(items_.begin(), items_.end(), [&oldName](const WheelItem& item) {
        return item.name == oldName;
    });

    if (itemIt == items_.end()) {
        throw WheelItemNotFoundException("Item not found: " + oldName);
    }

    itemIt->name = newName;
}

void Wheel::removeItem(const std::string& name) {
    const auto itemIt = std::find_if(items_.begin(), items_.end(), [&name](const WheelItem& item) {
        return item.name == name;
    });

    if (itemIt == items_.end()) {
        throw WheelItemNotFoundException("Item not found: " + name);
    }

    items_.erase(itemIt);
}

void Wheel::clear() { items_.clear(); }

void Wheel::reset() {
    for (auto& item : items_) {
        item.active = true;
    }
}

const std::string & Wheel::getName() const { return name_; }

const std::vector<WheelItem>& Wheel::items() const { return items_; }

const WheelItem& Wheel::itemAt(size_t index) const {
    if (index >= items_.size()) {
        throw WheelItemNotFoundException("Item index not found");
    }
    return items_[index];
}

bool Wheel::containsItem(const std::string& name) const {
    return std::any_of(items_.begin(), items_.end(), [&name](const WheelItem& item) {
        return item.name == name;
    });
}

void Wheel::deactivateItem(size_t index) {
    if (index >= items_.size()) {
        throw WheelItemNotFoundException("Item index not found");
    }
    items_[index].active = false;
}

void Wheel::setMode(WheelMode mode) { mode_ = mode; }

WheelMode Wheel::mode() const { return mode_; }
