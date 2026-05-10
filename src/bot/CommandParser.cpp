#include "CommandParser.hpp"

#include "BotCommands.hpp"

#include <algorithm>
#include <cctype>
#include <string_view>
#include <unordered_map>
#include <utility>

namespace {
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

std::string toLower(std::string value) {
    std::transform(value.begin(), value.end(), value.begin(), [](unsigned char ch) {
        return static_cast<char>(std::tolower(ch));
    });
    return value;
}

std::string_view trimView(std::string_view value) {
    while (!value.empty() && std::isspace(static_cast<unsigned char>(value.front()))) {
        value.remove_prefix(1);
    }
    while (!value.empty() && std::isspace(static_cast<unsigned char>(value.back()))) {
        value.remove_suffix(1);
    }
    return value;
}

BotCommandType commandTypeFromName(const std::string& commandName) {
    static const std::unordered_map<std::string_view, BotCommandType> commandTypes{
        {START_COMMAND, BotCommandType::Start},
        {HELP_COMMAND, BotCommandType::Help},
        {ITEM_ADD_COMMAND, BotCommandType::ItemAdd},
        {ITEM_EDIT_COMMAND, BotCommandType::ItemEdit},
        {ITEM_REMOVE_COMMAND, BotCommandType::ItemRemove},
        {ITEMS_LIST_COMMAND, BotCommandType::ItemsList},
        {WHEEL_SAVE_COMMAND, BotCommandType::WheelSave},
        {WHEEL_CHOOSE_COMMAND, BotCommandType::WheelChoose},
        {WHEEL_DELETE_COMMAND, BotCommandType::WheelDelete},
        {WHEEL_MODE_COMMAND, BotCommandType::WheelMode},
        {WHEEL_SPIN_COMMAND, BotCommandType::WheelSpin},
        {WHEEL_RESET_COMMAND, BotCommandType::WheelReset},
        {WHEEL_CLEAR_COMMAND, BotCommandType::WheelClear},
        {WHEELS_LIST_COMMAND, BotCommandType::WheelsList},
        {WHEEL_NEW_COMMAND, BotCommandType::WheelNew},
        {SECRET_COMMAND, BotCommandType::Secret}
    };

    const auto it = commandTypes.find(commandName);
    if (it == commandTypes.end()) {
        return BotCommandType::Unknown;
    }
    return it->second;
}
} // namespace

CommandParser::CommandParser(std::string botTag) : botTag_(std::move(botTag)) {}

std::optional<ParsedCommand> CommandParser::parse(const std::string& text) const {
    std::string_view rest = trimView(text);
    if (rest.empty()) {
        return std::nullopt;
    }

    if (rest.rfind(botTag_, 0) == 0) {
        rest.remove_prefix(botTag_.size());
    } else if (rest.front() == '/') {
        rest.remove_prefix(1);
    }
    rest = trimView(rest);

    const auto separatorPos = rest.find_first_of(std::string_view{":"});

    std::string rawCommand;
    std::optional<std::string> argument;
    if (separatorPos == std::string_view::npos) {
        rawCommand = trim(rest);
    } else {
        rawCommand = trim(rest.substr(0, separatorPos));
        argument = trim(rest.substr(separatorPos + 1));
    }

    const auto mentionSeparatorPos = rawCommand.find('@');
    if (mentionSeparatorPos != std::string::npos) {
        rawCommand.erase(mentionSeparatorPos);
    }

    ParsedCommand parsed;
    parsed.rawCommand = rawCommand;
    parsed.type = commandTypeFromName(toLower(rawCommand));
    parsed.argument = std::move(argument);

    return parsed;
}
