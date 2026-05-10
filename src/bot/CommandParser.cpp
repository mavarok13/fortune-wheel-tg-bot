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

BotCommandType commandTypeFromName(const std::string& commandName) {
    static const std::unordered_map<std::string_view, BotCommandType> commandTypes{
        {ITEM_ADD_COMMAND, BotCommandType::Add},
        {WHEEL_EDIT_COMMAND, BotCommandType::WheelEdit},
        {ITEM_EDIT_COMMAND, BotCommandType::ForEdit},
        {ITEM_REMOVE_COMMAND, BotCommandType::Delete},
        {WHEEL_SAVE_COMMAND, BotCommandType::WheelSave},
        {WHEEL_CHOOSE_COMMAND, BotCommandType::Choose},
        {WHEEL_MODE_COMMAND, BotCommandType::Mode},
        {WHEEL_SPIN_COMMAND, BotCommandType::Spin},
        {WHEEL_RESET_COMMAND, BotCommandType::Reset},
        {WHEEL_CLEAR_COMMAND, BotCommandType::Clear},
        {WHEEL_DELETE_COMMAND, BotCommandType::DeleteWheel},
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
    if (text.rfind(botTag_, 0) != 0) {
        return std::nullopt;
    }

    std::string_view rest(text.data() + botTag_.size(), text.size() - botTag_.size());
    rest = std::string_view(rest.data(), rest.size());
    const auto separatorPos = rest.find(CommandArgumentSeparator);

    std::string rawCommand;
    std::optional<std::string> argument;
    if (separatorPos == std::string_view::npos) {
        rawCommand = trim(rest);
    } else {
        rawCommand = trim(rest.substr(0, separatorPos));
        argument = trim(rest.substr(separatorPos + 1));
    }

    ParsedCommand parsed;
    parsed.rawCommand = rawCommand;
    parsed.type = commandTypeFromName(toLower(rawCommand));
    parsed.argument = std::move(argument);
    return parsed;
}
