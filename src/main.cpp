#include "bot/BotController.hpp"
#include "domain/WheelManager.hpp"
#include "storage/InMemoryStorage.hpp"
#include "telegram/TelegramClient.hpp"

#include <nlohmann/json.hpp>

#include <cstdlib>
#include <exception>
#include <iostream>
#include <optional>

int main() {
    const char* token = std::getenv("TELEGRAM_BOT_TOKEN");
    if (token == nullptr || std::string(token).empty()) {
        std::cerr << "TELEGRAM_BOT_TOKEN is not set." << std::endl;
        return 1;
    }

    try {
        TelegramClient client(token);
        WheelManager wheelManager;
        InMemoryStorage storage;
        BotController controller(client, wheelManager, storage);

        const auto me = client.getMe();
        std::cout << "getMe response: " << me.dump(2) << std::endl;

        std::optional<int> offset;
        while (true) {
            try {
                const auto updatesResponse = client.getUpdates(offset, 30);
                if (!updatesResponse.contains("result") || !updatesResponse["result"].is_array()) {
                    continue;
                }

                for (const auto& update : updatesResponse["result"]) {
                    if (update.contains("update_id") && update["update_id"].is_number_integer()) {
                        offset = update["update_id"].get<int>() + 1;
                    }
                    controller.handleUpdate(update);
                }
            } catch (const std::exception& ex) {
                std::cerr << "Polling iteration failed: " << ex.what() << std::endl;
            }
        }
    } catch (const std::exception& ex) {
        std::cerr << "Fatal error: " << ex.what() << std::endl;
        return 1;
    }

    return 0;
}
