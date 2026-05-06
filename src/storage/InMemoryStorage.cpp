#include "InMemoryStorage.hpp"

#include <stdexcept> 

Wheel & InMemoryStorage::getWheel(int64_t userId, int64_t wheelId) {
    std::string fullWheelId = std::to_string(userId) + "_" + std::to_string(wheelId);
    return wheels_[fullWheelId];
}

uint64_t InMemoryStorage::addWheel(int64_t userId, const Wheel & wheel) {
    uint64_t prevWheelId = lastWheelId_++;
    std::string fullWheelId = std::to_string(userId) + "_" + std::to_string(prevWheelId);
    wheels_[fullWheelId] = wheel;
    
    return prevWheelId;
}

void InMemoryStorage::removeWheel(int64_t userId, int64_t wheelId) {
    std::string fullWheelId = std::to_string(userId) + "_" + std::to_string(wheelId);
    const auto wheelIt = wheels_.find(fullWheelId);

    if (wheelIt != wheels_.end()) {
        wheels_.erase(wheelIt);
    } else {
        throw std::out_of_range("Wheel not found");
    }
}
