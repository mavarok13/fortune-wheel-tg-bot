#pragma once

#include <nlohmann/json.hpp>

class TelegramUpdateParser {
public:
    static bool isTextMessage(const nlohmann::json& update);
};
