#pragma once

#include "telegram/TelegramClient.hpp"
#include "domain/WheelManager.hpp"

#include <nlohmann/json.hpp>

class BotController {
public:
    explicit BotController(TelegramClient& telegramClient);

    void handleUpdate(const nlohmann::json& update);

private:
    TelegramClient& telegramClient_;
    WheelManager wheelManager_;
};
