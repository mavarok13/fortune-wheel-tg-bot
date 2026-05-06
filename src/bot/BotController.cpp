#include "BotController.hpp"
#include "BotCommands.hpp"

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
            // @auctionwheelbot itemadd: <item name>
            // @auctionwheelbot itemedit: <item name>
            // @auctionwheelbot itemremove: <item name>
            // @auctionwheelbot itemslist
            // @auctionwheelbot wheelsave: <wheel name>
            // @auctionwheelbot wheelchoose: <wheel name>
            // @auctionwheelbot wheelmode: <mode name>
            // @auctionwheelbot wheeldelete: <wheel name>
            // @auctionwheelbot wheelspin
            // @auctionwheelbot wheelreset
            // @auctionwheelbot wheelclear
            // @auctionwheelbot wheelslist
        }
    } catch (const std::exception& ex) {
        std::cerr << "Failed to handle update: " << ex.what() << std::endl;
    }
}
