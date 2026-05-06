#pragma once

#include "Storage.hpp"

#include <unordered_map>

class InMemoryStorage : public Storage {
public:
    Wheel & getWheel(int64_t userId, int64_t wheelId) override;
    uint64_t addWheel(int64_t userId, const Wheel & wheel) override;
    void removeWheel(int64_t userId, int64_t wheelId) override;

private:
    std::unordered_map<std::string, Wheel> wheels_;
    int64_t lastWheelId_ = 0;
};
