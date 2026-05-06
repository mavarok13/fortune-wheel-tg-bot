#pragma once

#include <cstdint>
#include <string>

// TODO: If shared group wheels are needed later, include chat.id or chat type in this key.
inline std::string makeWheelStorageId(int64_t userId, const std::string& wheelId) {
    return std::to_string(userId) + "_" + wheelId;
}
