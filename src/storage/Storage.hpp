#pragma once

#include "domain/Wheel.hpp"

#include <string>
#include <vector>

class Storage {
public:
    virtual ~Storage() = default;

    virtual Wheel& getWheel(const std::string& wheelStorageId) = 0;
    virtual Wheel& getTempWheel(const std::string& tempWheelId) = 0;
    virtual Wheel& getUserLastWheel(const std::string & userId) = 0;
    virtual void addWheel(const std::string& wheelStorageId, const Wheel& wheel) = 0;
    virtual void addTempWheel(const std::string& tempWheelId, const Wheel& wheel) = 0;
    virtual void setUserLastWheel(const std::string & userId, const std::string & wheelStorageId) = 0;
    virtual void removeWheel(const std::string& wheelStorageId) = 0;
    virtual void removeTempWheel(const std::string & tempWheelId) = 0;
    virtual bool wheelExists(const std::string & wheelStorageId) = 0;
    virtual bool tempWheelExists(const std::string & tempWheelIfd) = 0;
    virtual std::vector<std::string> listWheelNamesForUser(const std::string& userId) = 0;
};
