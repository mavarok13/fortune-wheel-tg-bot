#pragma once

#include "domain/Wheel.hpp"

#include <string>

class Storage {
public:
    virtual ~Storage() = default;

    virtual Wheel& getWheel(const std::string& wheelStorageId) = 0;
    virtual void addWheel(const std::string& wheelStorageId, const Wheel& wheel) = 0;
    virtual void removeWheel(const std::string& wheelStorageId) = 0;
};
