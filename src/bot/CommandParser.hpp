#pragma once

#include <optional>
#include <string>

inline constexpr char CommandArgumentSeparator = ':';

enum class BotCommandType {
    Add,
    WheelEdit,
    ForEdit,
    Delete,
    WheelSave,
    Choose,
    Mode,
    Spin,
    Reset,
    Clear,
    DeleteWheel,
    Unknown
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
