#pragma once

#include "domain/Wheel.hpp"

#include <cstdint>

class Storage {
public:
    virtual ~Storage() = default;

    virtual Wheel & getWheel(int64_t userId, int64_t wheelId) = 0;
    virtual uint64_t addWheel(int64_t userId, const Wheel & wheel) = 0;
    virtual void removeWheel(int64_t userId, int64_t wheelId) = 0;
};
