#pragma once

#include <optional>
#include <string>

inline constexpr char CommandArgumentSeparator = ':';

enum class BotCommandType {
    Start,
    Help,
    ItemAdd,
    ItemEdit,
    ItemRemove,
    ItemsList,
    WheelSave,
    WheelChoose,
    WheelMode,
    WheelSpin,
    WheelReset,
    WheelClear,
    WheelDelete,
    WheelsList,
    WheelNew,
    Unknown,
    Secret
};

struct ParsedCommand {
    BotCommandType type = BotCommandType::Unknown;
    std::string rawCommand;
    std::optional<std::string> argument;
};

class CommandParser {
public:
    explicit CommandParser(std::string botTag);

    std::optional<ParsedCommand> parse(const std::string& text) const;

private:
    std::string botTag_;
};
