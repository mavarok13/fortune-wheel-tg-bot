#include "TelegramClient.hpp"

#include <boost/beast/http.hpp>

#include <iostream>
#include <stdexcept>

TelegramClient::TelegramClient(std::string botToken)
    : token_(std::move(botToken)), http_("api.telegram.org") {}

nlohmann::json TelegramClient::getMe() {
    namespace http = boost::beast::http;
    HttpRequest request{http::verb::get, apiTarget("getMe"), 11};
    return performJsonRequest(std::move(request), "getMe");
}

nlohmann::json TelegramClient::getUpdates(std::optional<int> offset, int timeout) {
    namespace http = boost::beast::http;
    HttpRequest request{http::verb::post, apiTarget("getUpdates"), 11};
    request.set(http::field::content_type, "application/json");

    nlohmann::json body{{"timeout", timeout}};
    if (offset.has_value()) {
        body["offset"] = *offset;
    }

    request.body() = body.dump();
    request.prepare_payload();
    return performJsonRequest(std::move(request), "getUpdates");
}

nlohmann::json TelegramClient::sendMessage(int64_t chatId, const std::string& text) {
    namespace http = boost::beast::http;
    HttpRequest request{http::verb::post, apiTarget("sendMessage"), 11};
    request.set(http::field::content_type, "application/json");

    nlohmann::json body{{"chat_id", chatId}, {"text", text}};
    request.body() = body.dump();
    request.prepare_payload();

    return performJsonRequest(std::move(request), "sendMessage");
}

nlohmann::json TelegramClient::editMessageText(int64_t chatId, int messageId, const std::string& text) {
    namespace http = boost::beast::http;
    HttpRequest request{http::verb::post, apiTarget("editMessageText"), 11};
    request.set(http::field::content_type, "application/json");

    nlohmann::json body{{"chat_id", chatId}, {"message_id", messageId}, {"text", text}};
    request.body() = body.dump();
    request.prepare_payload();

    return performJsonRequest(std::move(request), "editMessageText");
}

nlohmann::json TelegramClient::performJsonRequest(HttpRequest request, const std::string& methodName) {
    auto response = http_.send(std::move(request));
    auto status = response.result_int();
    if (status < 200 || status >= 300) {
        std::cerr << "Telegram HTTP error for method " << methodName << ": status=" << status
                  << ", body=" << response.body() << std::endl;
        throw std::runtime_error("Telegram HTTP request failed");
    }

    auto parsed = nlohmann::json::parse(response.body());
    if (!parsed.value("ok", false)) {
        std::cerr << "Telegram API error for method " << methodName << ": " << parsed.dump() << std::endl;
        const auto description = parsed.value("description", std::string{"Unknown Telegram API error"});
        throw std::runtime_error("Telegram API error for method " + methodName + ": " + description);
    }
    return parsed;
}

std::string TelegramClient::apiTarget(const std::string& method) const {
    return "/bot" + token_ + "/" + method;
}
