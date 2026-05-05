#pragma once

#include "Storage.hpp"

#include <unordered_map>

class InMemoryStorage : public Storage {
public:
    Wheel& getWheel(int64_t chatId) override;

private:
    std::unordered_map<int64_t, Wheel> wheels_;
};
