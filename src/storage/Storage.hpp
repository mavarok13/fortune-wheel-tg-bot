#pragma once

#include "domain/Wheel.hpp"

#include <cstdint>

class Storage {
public:
    virtual ~Storage() = default;

    virtual Wheel& getWheel(int64_t chatId) = 0;
};
