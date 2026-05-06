#include "WheelManager.hpp"

#include "DomainExceptions.hpp"

#include <random>
#include <utility>
#include <vector>

void WheelManager::setCurrentWheel(Wheel& wheel) { currentWheel_ = &wheel; }

void WheelManager::clearCurrentWheel() { currentWheel_ = nullptr; }

bool WheelManager::hasCurrentWheel() const { return currentWheel_ != nullptr; }

Wheel& WheelManager::currentWheel() {
    if (!currentWheel_) {
        throw WheelNotSelectedException("Wheel not selected");
    }
    return *currentWheel_;
}

const Wheel& WheelManager::currentWheel() const {
    if (!currentWheel_) {
        throw WheelNotSelectedException("Wheel not selected");
    }
    return *currentWheel_;
}

void WheelManager::addItem(std::string name) { currentWheel().addItem(std::move(name)); }

void WheelManager::removeItem(const std::string& name) { currentWheel().removeItem(name); }

void WheelManager::reset() { currentWheel().reset(); }

void WheelManager::clear() { currentWheel().clear(); }

void WheelManager::setMode(WheelMode mode) { currentWheel().setMode(mode); }

std::optional<size_t> WheelManager::chooseIndex() {
    const auto& wheel = currentWheel();

    std::vector<size_t> active;
    const auto& items = wheel.items();
    for (size_t i = 0; i < items.size(); ++i) {
        if (items[i].active) {
            active.push_back(i);
        }
    }

    if (active.empty()) {
        return std::nullopt;
    }

    static thread_local std::mt19937 rng{std::random_device{}()};
    std::uniform_int_distribution<size_t> dist(0, active.size() - 1);
    return active[dist(rng)];
}
