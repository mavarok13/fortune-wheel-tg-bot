#include "BotController.hpp"

#include "BotCommands.hpp"
#include "domain/DomainExceptions.hpp"
#include "storage/WheelStorageKey.hpp"

#include <algorithm>
#include <cctype>
#include <iostream>
#include <stdexcept>
#include <string>

namespace {
std::string toLower(std::string value) {
    std::transform(value.begin(), value.end(), value.begin(), [](unsigned char ch) {
        return static_cast<char>(std::tolower(ch));
    });
    return value;
}

WheelMode wheelModeFromArgument(const std::string& value) {
    const auto mode = toLower(value);
    if (mode == "choice") {
        return WheelMode::Choice;
    }
    if (mode == "elimination") {
        return WheelMode::Elimination;
    }
    throw InvalidWheelOperationException("Unknown wheel mode: " + value);
}

std::string wheelModeToString(WheelMode mode) {
    switch (mode) {
    case WheelMode::Choice:
        return "choice";
    case WheelMode::Elimination:
        return "elimination";
    }
    return "choice";
}
} // namespace

BotController::BotController(TelegramClient& telegramClient, WheelManager& wheelManager, Storage& storage)
    : telegramClient_(telegramClient),
      wheelManager_(wheelManager),
      storage_(storage),
      commandParser_(std::string(COMMAND_BEGINNING)) {}

void BotController::handleUpdate(const nlohmann::json& update) {
    int64_t chatId = 0;
    try {
        if (!update.contains("message") || !update["message"].is_object()) {
            // TODO: Later, actual Telegram inline queries may require handling update.inline_query.
            // TODO: Autocomplete/dropdown-like behavior may require InlineQueryResult responses.
            return;
        }

        const auto& message = update["message"];
        if (!message.contains("chat") || !message["chat"].contains("id")) {
            return;
        }

        chatId = message["chat"]["id"].get<int64_t>();

        if (!message.contains("text") || !message["text"].is_string()) {
            return;
        }

        const auto parsedCommand = commandParser_.parse(message["text"].get<std::string>());
        if (!parsedCommand.has_value()) {
            return;
        }

        if (!message.contains("from") || !message["from"].contains("id")) {
            reply(chatId, "Cannot determine user for this command.");
            return;
        }

        // For now wheels are user-scoped, so update.message.from.id owns the wheel.
        // TODO: If shared group wheels are needed later, storage key should include chat.id or chat type.
        const int64_t userId = message["from"]["id"].get<int64_t>();
        handleCommand(chatId, userId, *parsedCommand);
    } catch (const WheelNotSelectedException& ex) {
        std::cerr << "Wheel selection error: " << ex.what() << std::endl;
        if (chatId != 0) {
            reply(chatId, "No wheel selected. Create or choose a wheel first.");
        }
    } catch (const WheelItemNotFoundException& ex) {
        std::cerr << "Wheel item error: " << ex.what() << std::endl;
        if (chatId != 0) {
            reply(chatId, ex.what());
        }
    } catch (const WheelNotFoundException& ex) {
        std::cerr << "Wheel lookup error: " << ex.what() << std::endl;
        if (chatId != 0) {
            reply(chatId, ex.what());
        }
    } catch (const InvalidWheelOperationException& ex) {
        std::cerr << "Invalid wheel operation: " << ex.what() << std::endl;
        if (chatId != 0) {
            reply(chatId, ex.what());
        }
    } catch (const std::runtime_error& ex) {
        std::cerr << "Runtime error while handling update: " << ex.what() << std::endl;
        if (chatId != 0) {
            reply(chatId, ex.what());
        }
    } catch (const std::exception& ex) {
        std::cerr << "Failed to handle update: " << ex.what() << std::endl;
        if (chatId != 0) {
            reply(chatId, "Sorry, something went wrong.");
        }
    }
}

void BotController::handleCommand(int64_t chatId, int64_t userId, const ParsedCommand& command) {
    switch (command.type) {
    case BotCommandType::Add: {
        const auto itemName = requireArgument(command, "add");
        selectCurrentWheelForUser(userId);
        wheelManager_.addItem(itemName);
        reply(chatId, "Added item: " + itemName);
        return;
    }
    case BotCommandType::WheelSave: {
        const auto wheelName = requireArgument(command, "wheelsave");
        Wheel wheel;
        const auto storageId = makeWheelStorageId(userId, wheelName);
        storage_.addWheel(storageId, wheel);
        wheelManager_.setCurrentWheel(storage_.getWheel(storageId));
        currentWheelStorageIds_[userId] = storageId;
        reply(chatId, "Wheel saved and selected: " + wheelName);
        return;
    }
    case BotCommandType::Choose: {
        const auto wheelName = requireArgument(command, "choose");
        const auto storageId = makeWheelStorageId(userId, wheelName);
        try {
            wheelManager_.setCurrentWheel(storage_.getWheel(storageId));
        } catch (const WheelNotFoundException&) {
            reply(chatId, "Wheel not found: " + wheelName);
            return;
        }
        currentWheelStorageIds_[userId] = storageId;
        reply(chatId, "Selected wheel: " + wheelName);
        return;
    }
    case BotCommandType::DeleteWheel: {
        const auto wheelName = requireArgument(command, "deletewheel");
        const auto storageId = makeWheelStorageId(userId, wheelName);
        try {
            storage_.removeWheel(storageId);
        } catch (const WheelNotFoundException&) {
            reply(chatId, "Wheel not found: " + wheelName);
            return;
        }
        const auto currentIt = currentWheelStorageIds_.find(userId);
        if (currentIt != currentWheelStorageIds_.end() && currentIt->second == storageId) {
            wheelManager_.clearCurrentWheel();
            currentWheelStorageIds_.erase(currentIt);
        }
        reply(chatId, "Deleted wheel: " + wheelName);
        return;
    }
    case BotCommandType::Delete: {
        const auto itemName = requireArgument(command, "delete");
        selectCurrentWheelForUser(userId);
        try {
            wheelManager_.removeItem(itemName);
        } catch (const WheelItemNotFoundException&) {
            reply(chatId, "Item not found: " + itemName);
            return;
        }
        reply(chatId, "Deleted item: " + itemName);
        return;
    }
    case BotCommandType::ForEdit: {
        const auto itemName = requireArgument(command, "foredit");
        selectCurrentWheelForUser(userId);
        if (!wheelManager_.currentWheel().containsItem(itemName)) {
            reply(chatId, "Item not found: " + itemName);
            return;
        }
        // TODO: Add per-user pending state for PendingAction::EditItemName.
        reply(chatId, "Editing is not interactive yet. Later this command will ask for a new item name.");
        return;
    }
    case BotCommandType::Mode: {
        const auto modeArgument = requireArgument(command, "mode");
        const auto mode = wheelModeFromArgument(modeArgument);
        selectCurrentWheelForUser(userId);
        wheelManager_.setMode(mode);
        reply(chatId, "Mode set to: " + wheelModeToString(mode));
        return;
    }
    case BotCommandType::Spin: {
        selectCurrentWheelForUser(userId);
        const auto selectedIndex = wheelManager_.chooseIndex();
        if (!selectedIndex.has_value()) {
            reply(chatId, "No active items to select.");
            return;
        }

        Wheel& wheel = wheelManager_.currentWheel();
        const auto selectedItemName = wheel.itemAt(*selectedIndex).name;
        if (wheel.mode() == WheelMode::Elimination) {
            wheel.deactivateItem(*selectedIndex);
        }
        // TODO: Later this should use SpinAnimationService and editMessageText.
        reply(chatId, "Selected: " + selectedItemName);
        return;
    }
    case BotCommandType::Reset:
        selectCurrentWheelForUser(userId);
        wheelManager_.reset();
        reply(chatId, "Wheel reset.");
        return;
    case BotCommandType::Clear:
        selectCurrentWheelForUser(userId);
        wheelManager_.clear();
        reply(chatId, "Wheel cleared.");
        return;
    case BotCommandType::WheelEdit:
    case BotCommandType::Unknown:
        reply(chatId, "Unknown command.");
        return;
    }
}

Wheel& BotController::selectCurrentWheelForUser(int64_t userId) {
    const auto currentIt = currentWheelStorageIds_.find(userId);
    if (currentIt == currentWheelStorageIds_.end()) {
        throw WheelNotSelectedException("Wheel not selected");
    }

    Wheel& wheel = storage_.getWheel(currentIt->second);
    wheelManager_.setCurrentWheel(wheel);
    return wheel;
}

std::string BotController::requireArgument(const ParsedCommand& command, const std::string& commandName) const {
    if (!command.argument.has_value() || command.argument->empty()) {
        throw InvalidWheelOperationException("Missing argument for command: " + commandName);
    }
    return *command.argument;
}

void BotController::reply(int64_t chatId, const std::string& text) { telegramClient_.sendMessage(chatId, text); }
