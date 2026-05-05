#pragma once

#include "telegram/TelegramClient.hpp"

#include <nlohmann/json.hpp>

class BotController {
public:
    explicit BotController(TelegramClient& telegramClient);

    void handleUpdate(const nlohmann::json& update);

private:
    TelegramClient& telegramClient_;
};
