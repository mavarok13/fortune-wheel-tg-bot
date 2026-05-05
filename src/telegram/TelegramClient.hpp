#pragma once

#include "http/HttpClient.hpp"

#include <nlohmann/json.hpp>

#include <cstdint>
#include <optional>
#include <string>

class TelegramClient {
public:
    explicit TelegramClient(std::string botToken);

    nlohmann::json getMe();

    nlohmann::json getUpdates(std::optional<int> offset = std::nullopt, int timeout = 30);

    nlohmann::json sendMessage(int64_t chatId, const std::string& text);

    nlohmann::json editMessageText(int64_t chatId, int messageId, const std::string& text);

private:
    nlohmann::json performJsonRequest(HttpRequest request, const std::string& methodName);
    std::string apiTarget(const std::string& method) const;

    std::string token_;
    HttpClient http_;
};
