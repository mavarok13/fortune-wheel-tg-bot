#include "InMemoryStorage.hpp"

#include "domain/DomainExceptions.hpp"

Wheel& InMemoryStorage::getWheel(const std::string& wheelStorageId) {
    const auto wheelIt = wheels_.find(wheelStorageId);
    if (wheelIt == wheels_.end()) {
        throw WheelNotFoundException("Wheel not found: " + wheelStorageId);
    }
    return wheelIt->second;
}

void InMemoryStorage::addWheel(const std::string& wheelStorageId, const Wheel& wheel) {
    wheels_[wheelStorageId] = wheel;
    ++lastIWheelId_;
}

void InMemoryStorage::removeWheel(const std::string& wheelStorageId) {
    const auto wheelIt = wheels_.find(wheelStorageId);
    if (wheelIt == wheels_.end()) {
        throw WheelNotFoundException("Wheel not found: " + wheelStorageId);
    }
    wheels_.erase(wheelIt);
}
