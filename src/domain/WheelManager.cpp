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

void WheelManager::addItem(std::string name, std::uint64_t score) { currentWheel().addItem(std::move(name), score); }

void WheelManager::setItemScore(const std::string& name, std::uint64_t score) {
    currentWheel().setItemScore(name, score);
}

void WheelManager::addItemScore(const std::string& name, std::uint64_t scoreDelta) {
    currentWheel().addItemScore(name, scoreDelta);
}

void WheelManager::removeItem(const std::string& name) { currentWheel().removeItem(name); }

void WheelManager::reset() { currentWheel().reset(); }

void WheelManager::clear() { currentWheel().clear(); }

void WheelManager::setMode(WheelMode mode) { currentWheel().setMode(mode); }

std::optional<size_t> WheelManager::chooseIndex() {
    const auto& wheel = currentWheel();

    std::vector<size_t> active;
    std::vector<double> weights;
    const auto& items = wheel.items();
    for (size_t i = 0; i < items.size(); ++i) {
        if (items[i].active) {
            active.push_back(i);
            if (wheel.mode() == WheelMode::Elimination) {
                weights.push_back(1.0 / static_cast<double>(items[i].score));
            } else {
                weights.push_back(static_cast<double>(items[i].score));
            }
        }
    }

    if (active.empty()) {
        return std::nullopt;
    }

    static thread_local std::mt19937 rng{std::random_device{}()};
    std::discrete_distribution<size_t> dist(weights.begin(), weights.end());
    return active[dist(rng)];
}
