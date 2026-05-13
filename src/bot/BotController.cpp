#include "BotController.hpp"

#include "BotCommands.hpp"
#include "domain/DomainExceptions.hpp"
#include "storage/WheelStorageKey.hpp"

#include <algorithm>
#include <cctype>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>

namespace {
std::string commandPrefix() {
    return std::string(COMMAND_BEGINNING.data(), COMMAND_BEGINNING.size());
}

std::string escapeHtml(std::string_view value) {
    std::string escaped;
    escaped.reserve(value.size());

    for (const char ch : value) {
        switch (ch) {
        case '&':
            escaped += "&amp;";
            break;
        case '<':
            escaped += "&lt;";
            break;
        case '>':
            escaped += "&gt;";
            break;
        case '\"':
            escaped += "&quot;";
            break;
        default:
            escaped += ch;
            break;
        }
    }

    return escaped;
}

std::string code(std::string_view value) {
    return "<code>" + escapeHtml(value) + "</code>";
}

std::string bold(std::string_view value) {
    return "<b>" + escapeHtml(value) + "</b>";
}

std::string wheelNotSelectedMessage() {
    return "No wheel selected yet. Try " + code("/wheelnew") + " or " + code("/wheelchoose your_wheel") + ".";
}

std::string formatHelpMessage() {
    const auto prefix = commandPrefix();
    std::ostringstream message;
    message << "<b>Welcome to " << escapeHtml(prefix) << "</b>\n";
    message << "Let the wheel handle the dramatic decision-making.\n\n";
    message << bold("How to talk to me") << "\n";
    message << code(prefix + " command: argument") << " or " << code("/command argument") << "\n\n";
    message << bold("Commands") << "\n";
    message << code("help") << " - show all commands\n";
    message << code("itemadd item_name") << " - add a new item\n";
    message << code("itemedit old_name:new_name") << " - rename an item\n";
    message << code("itemremove item_name") << " - remove an item\n";
    message << code("itemslist") << " - show items in the current wheel\n";
    message << code("wheelnew") << " - create a fresh wheel\n";
    message << code("wheelsave wheel_name") << " - save the current wheel\n";
    message << code("wheelchoose wheel_name") << " - select a saved wheel\n";
    message << code("wheeldelete wheel_name") << " - delete a saved wheel\n";
    message << code("wheelmode choice") << " or " << code("wheelmode elimination") << " - choose the game mode\n";
    message << code("wheelspin") << " - spin the wheel\n";
    message << code("wheelreset") << " - reactivate excluded items\n";
    message << code("wheelclear") << " - remove all items from the current wheel\n";
    message << code("wheelslist") << " - list saved wheels";
    return message.str();
}

std::string toLower(std::string value) {
    std::transform(value.begin(), value.end(), value.begin(), [](unsigned char ch) {
        return static_cast<char>(std::tolower(ch));
    });
    return value;
}

std::string trim(std::string_view value) {
    auto begin = value.begin();
    auto end = value.end();

    while (begin != end && std::isspace(static_cast<unsigned char>(*begin))) {
        ++begin;
    }
    while (begin != end && std::isspace(static_cast<unsigned char>(*(end - 1)))) {
        --end;
    }

    return std::string(begin, end);
}

std::pair<std::string, std::string> parseItemRenameArgument(const std::string& value) {
    const auto separatorPos = value.find(CommandArgumentSeparator);
    if (separatorPos == std::string::npos) {
        throw InvalidWheelOperationException("Use itemedit:old name:new name");
    }

    const auto oldName = trim(value.substr(0, separatorPos));
    const auto newName = trim(value.substr(separatorPos + 1));
    if (oldName.empty() || newName.empty()) {
        throw InvalidWheelOperationException("Use itemedit:old name:new name");
    }

    return {oldName, newName};
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
            reply(chatId, wheelNotSelectedMessage());
        }
    } catch (const WheelItemNotFoundException& ex) {
        std::cerr << "Wheel item error: " << ex.what() << std::endl;
        if (chatId != 0) {
            reply(chatId, escapeHtml(ex.what()));
        }
    } catch (const WheelNotFoundException& ex) {
        std::cerr << "Wheel lookup error: " << ex.what() << std::endl;
        if (chatId != 0) {
            reply(chatId, escapeHtml(ex.what()));
        }
    } catch (const InvalidWheelOperationException& ex) {
        std::cerr << "Invalid wheel operation: " << ex.what() << std::endl;
        if (chatId != 0) {
            reply(chatId, escapeHtml(ex.what()));
        }
    } catch (const std::runtime_error& ex) {
        std::cerr << "Runtime error while handling update: " << ex.what() << std::endl;
        if (chatId != 0) {
            reply(chatId, escapeHtml(ex.what()));
        }
    } catch (const std::exception& ex) {
        std::cerr << "Failed to handle update: " << ex.what() << std::endl;
        if (chatId != 0) {
            reply(chatId, "Sorry, something went wrong.");
        }
    }
}

void BotController::handleCommand(int64_t chatId, int64_t userId, const ParsedCommand& command) {
    const std::string userKey = std::to_string(userId);
    const auto selectLastWheelForUser = [&]() -> bool {
        try {
            wheelManager_.setCurrentWheel(storage_.getUserLastWheel(userKey));
            return true;
        } catch (const WheelNotFoundException&) {
            wheelManager_.clearCurrentWheel();
            return false;
        }
    };

    switch (command.type) {
    case BotCommandType::Start:
    case BotCommandType::Help: {
        reply(chatId, formatHelpMessage());
        return;
    }
    case BotCommandType::Secret: {
        reply(chatId, "<b>Я ЛЮБЛЮ ТЕБЯ, МОЯ КИСОНЬКА!</b> &lt;3");
        return;
    }
    case BotCommandType::ItemAdd: {
        if (!selectLastWheelForUser()) {
            storage_.addTempWheel(userKey, Wheel{});
            setCurrentWheel(userKey, userKey, true);
        }

        const auto itemName = requireArgument(command);
        wheelManager_.addItem(itemName);
        const auto wheelName = wheelManager_.currentWheel().getName();
        if (wheelName.empty()) {
            reply(chatId, "Added " + code(itemName) + " to the wheel.");
        } else {
            reply(chatId, "Added " + code(itemName) + " to wheel " + code(wheelName) + ".");
        }
        return;
    }
    case BotCommandType::ItemEdit: {
        if (!selectLastWheelForUser()) {
            reply(chatId, wheelNotSelectedMessage());
            return;
        }

        const auto renameArgument = requireArgument(command);
        const auto [oldName, newName] = parseItemRenameArgument(renameArgument);
        wheelManager_.currentWheel().renameItem(oldName, newName);
        reply(chatId, "Renamed " + code(oldName) + " to " + code(newName) + ".");
        return;
    }
    case BotCommandType::ItemRemove: {
        if (!selectLastWheelForUser()) {
            reply(chatId, wheelNotSelectedMessage());
            return;
        }

        const auto itemName = requireArgument(command);
        wheelManager_.removeItem(itemName);
        reply(chatId, "Removed " + code(itemName) + ".");
        return;
    }
    case BotCommandType::WheelSave: {
        if (!selectLastWheelForUser()) {
            reply(chatId, wheelNotSelectedMessage());
            return;
        }

        auto & currentWheel = wheelManager_.currentWheel();
        std::string wheelName = requireArgument(command);
        const auto wheelId = makeWheelStorageId(userId, wheelName);
        currentWheel.setName(wheelName);
        storage_.addWheel(wheelId, currentWheel);
        setCurrentWheel(userKey, wheelId);
        reply(chatId, "Saved and selected wheel " + code(wheelName) + ".");
        return;
    }
    case BotCommandType::ItemsList: {
        if (!selectLastWheelForUser()) {
            reply(chatId, wheelNotSelectedMessage());
            return;
        }

        auto & wheel = wheelManager_.currentWheel();
        if (wheel.items().empty()) {
            reply(chatId, "Wheel " + code(wheel.getName()) + " has no items yet.");
            return;
        }

        std::string itemsListStr = bold("Items in " + wheel.getName()) + "\n";
        for (const auto & item : wheel.items()) {
            itemsListStr += "- " + code(item.name);
            if (!item.active) {
                itemsListStr += " <i>(inactive)</i>";
            }
            itemsListStr += "\n";
        }

        reply(chatId, itemsListStr);
        return;
    }
    case BotCommandType::WheelChoose: {
        const auto wheelName = requireArgument(command);
        const auto storageId = makeWheelStorageId(userId, wheelName);
        try {
            setCurrentWheel(userKey, storageId);
        } catch (const WheelNotFoundException&) {
            reply(chatId, "Wheel not found: " + code(wheelName));
            return;
        }
        reply(chatId, "Selected wheel " + code(wheelName) + ".");
        return;
    }
    case BotCommandType::WheelMode: {
        if (!selectLastWheelForUser()) {
            reply(chatId, wheelNotSelectedMessage());
            return;
        }

        const auto modeArgument = requireArgument(command);
        const auto mode = wheelModeFromArgument(modeArgument);
        wheelManager_.setMode(mode);
        reply(chatId, "Mode set to " + code(wheelModeToString(mode)) + ".");
        return;
    }
    case BotCommandType::WheelSpin: {
        if (!selectLastWheelForUser()) {
            reply(chatId, wheelNotSelectedMessage());
            return;
        }

        const auto selectedIndex = wheelManager_.chooseIndex();
        if (!selectedIndex.has_value()) {
            reply(chatId, "No active items to select.");
            return;
        }

        Wheel& wheel = wheelManager_.currentWheel();
        const auto selectedItemName = wheel.itemAt(*selectedIndex).name;
        if (wheel.mode() == WheelMode::Elimination) {
            wheel.deactivateItem(*selectedIndex);

            int activeItemsCount = 0;
            const WheelItem * lastActiveItem = nullptr;
            for(const auto & item : wheel.items()) {
                if (item.active) {
                    lastActiveItem = &item;
                    ++activeItemsCount;
                }
            }
            
            if (activeItemsCount <= 1) {
                wheel.reset();
                if (lastActiveItem) {
                    reply(chatId, bold("Excluded") + ": " + code(selectedItemName) + "\n" +
                        bold("Winner") + ": " + code(lastActiveItem->name) + "\n<i>(wheel has been reset)</i>");
                } else {
                    reply(chatId, bold("Winner") + ": " + code(selectedItemName) + "\n<i>(wheel has been reset)</i>");
                }
                return;
            }

            std::string eliminationReplyMsg = bold("Excluded") + ": " + code(selectedItemName) + "\n" + bold("Remaining") + "\n";
            for (const auto & item : wheel.items()) {
                if (item.active) {
                    eliminationReplyMsg += "- " + code(item.name) + "\n";
                }
            }
            eliminationReplyMsg.erase(eliminationReplyMsg.end()-1);
            reply(chatId, eliminationReplyMsg);
            return;
        }
        reply(chatId, bold("Selected") + ": " + code(selectedItemName));

        return;
    }
    case BotCommandType::WheelReset:
        if (!selectLastWheelForUser()) {
            reply(chatId, wheelNotSelectedMessage());
            return;
        }

        wheelManager_.reset();
        reply(chatId, "Wheel reset. Everybody is back in the game.");
        return;
    case BotCommandType::WheelClear:
        if (!selectLastWheelForUser()) {
            reply(chatId, wheelNotSelectedMessage());
            return;
        }

        wheelManager_.clear();
        reply(chatId, "Wheel cleared. A clean slate awaits.");
        return;
    case BotCommandType::WheelDelete: {
        const auto wheelName = requireArgument(command);
        const auto storageId = makeWheelStorageId(userId, wheelName);
        const bool deletedCurrentWheel = selectLastWheelForUser() && wheelManager_.currentWheel().getName() == wheelName;

        try {
            storage_.removeWheel(storageId);
        } catch (const WheelNotFoundException&) {
            reply(chatId, "Wheel not found: " + code(wheelName));
            return;
        }

        if (deletedCurrentWheel) {
            storage_.addTempWheel(userKey, Wheel{});
            setCurrentWheel(userKey, userKey, true);
        }

        reply(chatId, "Deleted wheel " + code(wheelName) + ".");
        return;
    }
    case BotCommandType::WheelNew: {
        storage_.addTempWheel(userKey, Wheel{});
        std::string wheelId = userKey;
        setCurrentWheel(userKey, wheelId, true);
        reply(chatId, "Fresh wheel created. Start feeding it choices.");
        return;
    }
    case BotCommandType::WheelsList: {
        const auto wheelNames = storage_.listWheelNamesForUser(userKey);
        if (wheelNames.empty()) {
            reply(chatId, "No saved wheels yet.");
            return;
        }

        std::string currentWheelName;
        if (selectLastWheelForUser()) {
            currentWheelName = wheelManager_.currentWheel().getName();
        }

        std::string wheelList = bold("Saved wheels") + "\n";
        for (const auto& wheelName : wheelNames) {
            wheelList += "- " + code(wheelName);
            if (wheelName == currentWheelName) {
                wheelList += " <i>(selected)</i>";
            }
            wheelList += "\n";
        }

        reply(chatId, wheelList);
        return;
    }
    case BotCommandType::Unknown:
        reply(chatId, "Unknown command. Try " + code("/help") + ".");
        return;
    }
}

void BotController::setCurrentWheel(const std::string & userId, const std::string & wheelId, bool isTemp) {
    if (isTemp) {
        Wheel& wheel = storage_.getTempWheel(wheelId);
        wheelManager_.setCurrentWheel(wheel);
    } else {
        Wheel& wheel = storage_.getWheel(wheelId);
        wheelManager_.setCurrentWheel(wheel);
    }

    storage_.setUserLastWheel(userId, wheelId);
}

std::string BotController::requireArgument(const ParsedCommand& command) const {
    if (!command.argument.has_value() || command.argument->empty()) {
        throw InvalidWheelOperationException("Missing argument for command. Check /help for the expected format.");
    }
    return *command.argument;
}

void BotController::reply(int64_t chatId, const std::string& text) {
    std::cerr << "Replying to chat " << chatId << " with text=" << text << std::endl;
    telegramClient_.sendMessage(chatId, text);
}
