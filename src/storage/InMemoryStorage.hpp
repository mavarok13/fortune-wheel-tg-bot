#pragma once

#include "Storage.hpp"

#include <cstdint>
#include <unordered_map>

class InMemoryStorage : public Storage {
public:
    Wheel& getWheel(const std::string& wheelStorageId) override;
    void addWheel(const std::string& wheelStorageId, const Wheel& wheel) override;
    void removeWheel(const std::string& wheelStorageId) override;

private:
    std::unordered_map<std::string, Wheel> wheels_;
    int64_t lastIWheelId_ = 0;
};
