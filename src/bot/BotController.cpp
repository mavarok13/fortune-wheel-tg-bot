#include "BotController.hpp"

#include <iostream>

BotController::BotController(TelegramClient& telegramClient) : telegramClient_(telegramClient) {}

void BotController::handleUpdate(const nlohmann::json& update) {
    try {
        if (!update.contains("message") || !update["message"].is_object()) {
            return;
        }

        const auto& message = update["message"];
        if (!message.contains("chat") || !message["chat"].contains("id")) {
            return;
        }

        const int64_t chatId = message["chat"]["id"].get<int64_t>();

        if (message.contains("text") && message["text"].is_string()) {
            // TODO: Implement command parsing and inline mode support:
            // @fortunewheel add: <name>
            // @fortunewheel wheeledit:
            // @fortunewheel foredit: <name>
            // @fortunewheel delete: <name>
            // @fortunewheel wheelsave: <name>
            // @fortunewheel choose
            // @fortunewheel choose: <name>
            // @fortunewheel mode
            // @fortunewheel spin
            // @fortunewheel reset
            // @fortunewheel clear
            // @fortunewheel deletewheel
            telegramClient_.sendMessage(chatId, "FortuneWheelBot is running. Command parsing will be implemented later.");
        }
    } catch (const std::exception& ex) {
        std::cerr << "Failed to handle update: " << ex.what() << std::endl;
    }
}
