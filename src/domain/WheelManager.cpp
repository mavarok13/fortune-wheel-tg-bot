#include "WheelManager.hpp"

#include <random>
#include <vector>
#include <stdexcept> 

std::optional<size_t> WheelManager::chooseIndex() {
    if (!currentWheel_) {
        throw std::logic_error("Wheel not selected");
    }

    std::vector<size_t> active;
    const auto& items = currentWheel_->items();
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
