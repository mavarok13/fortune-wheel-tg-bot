#pragma once

#include <cstdint>
#include <string>

struct WheelItem {
    std::string id;
    std::string name;
    bool active = true;
    std::uint64_t score = 1;
};
