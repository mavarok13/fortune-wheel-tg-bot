#pragma once

#include "CommandParser.hpp"
#include "domain/WheelManager.hpp"
#include "storage/Storage.hpp"
#include "telegram/TelegramClient.hpp"

#include <nlohmann/json.hpp>

#include <optional>
#include <string>
#include <unordered_map>

class BotController {
public:
    BotController(TelegramClient& telegramClient, WheelManager& wheelManager, Storage& storage);

    void handleUpdate(const nlohmann::json& update);

private:
    void handleCommand(int64_t chatId, int64_t userId, const ParsedCommand& command);
    std::string requireArgument(const ParsedCommand& command, const std::string& commandName) const;
    Wheel& selectCurrentWheelForUser(int64_t userId);
    void reply(int64_t chatId, const std::string& text);

    TelegramClient& telegramClient_;
    WheelManager& wheelManager_;
    Storage& storage_;
    CommandParser commandParser_;
    std::unordered_map<int64_t, std::string> currentWheelStorageIds_;
};
