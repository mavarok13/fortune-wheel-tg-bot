#pragma once

#include "Storage.hpp"

#include <cstdint>
#include <unordered_map>

class InMemoryStorage : public Storage {
public:
    Wheel& getWheel(const std::string& wheelStorageId) override;
    Wheel& getTempWheel(const std::string& tempWheelId) override;
    Wheel& getUserLastWheel(const std::string& userId) override;
    void addWheel(const std::string& wheelStorageId, const Wheel& wheel) override;
    void addTempWheel(const std::string& tempWheelId, const Wheel& wheel) override;
    void setUserLastWheel(const std::string & userId, const std::string & wheelStorageId) override;
    void removeWheel(const std::string& wheelStorageId) override;
    void removeTempWheel(const std::string & tempWheelId) override;

    bool wheelExists(const std::string & wheelStorageId) override;
    bool tempWheelExists(const std::string & tempWheelId) override;
    std::vector<std::string> listWheelNamesForUser(const std::string& userId) override;
private:
    std::unordered_map<std::string, Wheel> wheels_;
    std::unordered_map<std::string, Wheel> tempWheels_;
    std::unordered_map<std::string, std::string> lastUsersWheels_;
};
