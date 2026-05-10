#include "TelegramUpdateParser.hpp"

bool TelegramUpdateParser::isTextMessage(const nlohmann::json& update) {
    return update.contains("message") && update["message"].contains("text") && update["message"]["text"].is_string();
}
