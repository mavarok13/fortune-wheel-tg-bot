#include "InMemoryStorage.hpp"

#include "domain/DomainExceptions.hpp"

#include <algorithm>

Wheel& InMemoryStorage::getWheel(const std::string& wheelStorageId) {
    const auto wheelIt = wheels_.find(wheelStorageId);
    if (wheelIt == wheels_.end()) {
        throw WheelNotFoundException("Wheel not found: " + wheelStorageId);
    }
    return wheelIt->second;
}
Wheel& InMemoryStorage::getTempWheel(const std::string& tempWheelId) {
    const auto wheelIt = tempWheels_.find(tempWheelId);
    if (wheelIt == tempWheels_.end()) {
        throw WheelNotFoundException("Wheel not found: " + tempWheelId);
    }
    return wheelIt->second;
}
Wheel & InMemoryStorage::getUserLastWheel(const std::string & userId) {
    const auto wheelIt = lastUsersWheels_.find(userId);
    if (wheelIt == lastUsersWheels_.end()) {
        throw WheelNotFoundException("No last wheel for user: " + userId);
    }

    Wheel & lastUserWheel = wheelIt->first == wheelIt->second ? getTempWheel(wheelIt->second) : getWheel(wheelIt->second);
    return lastUserWheel;
}

void InMemoryStorage::addWheel(const std::string& wheelStorageId, const Wheel& wheel) {
    wheels_[wheelStorageId] = wheel;
}
void InMemoryStorage::addTempWheel(const std::string& tempWheelId, const Wheel& wheel) {
    tempWheels_[tempWheelId] = wheel;
}
void InMemoryStorage::setUserLastWheel(const std::string & userId, const std::string & wheelStorageId) {
    lastUsersWheels_[userId] = wheelStorageId;
}

void InMemoryStorage::removeWheel(const std::string& wheelStorageId) {
    const auto wheelIt = wheels_.find(wheelStorageId);
    if (wheelIt == wheels_.end()) {
        throw WheelNotFoundException("Wheel not found: " + wheelStorageId);
    }
    wheels_.erase(wheelIt);
}
void InMemoryStorage::removeTempWheel(const std::string& tempWheelId) {
    const auto wheelIt = tempWheels_.find(tempWheelId);
    if (wheelIt == tempWheels_.end()) {
        throw WheelNotFoundException("Wheel not found: " + tempWheelId);
    }
    tempWheels_.erase(wheelIt);
}

bool InMemoryStorage::wheelExists(const std::string & wheelStorageId) {
    const auto wheelIt = wheels_.find(wheelStorageId);
    return wheelIt != wheels_.end();
}

bool InMemoryStorage::tempWheelExists(const std::string & tempWheelId) {
    const auto wheelIt = tempWheels_.find(tempWheelId);
    return wheelIt != tempWheels_.end();
}

std::vector<std::string> InMemoryStorage::listWheelNamesForUser(const std::string& userId) {
    const std::string prefix = userId + "_";
    std::vector<std::string> wheelNames;

    for (const auto& wheelEntry : wheels_) {
        const auto& storageId = wheelEntry.first;
        if (storageId.rfind(prefix, 0) == 0) {
            wheelNames.push_back(storageId.substr(prefix.size()));
        }
    }

    std::sort(wheelNames.begin(), wheelNames.end());
    return wheelNames;
}
