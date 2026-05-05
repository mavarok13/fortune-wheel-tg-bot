cmake_minimum_required(VERSION 3.16)
project(FortuneWheelBot LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Boost REQUIRED COMPONENTS system)
find_package(OpenSSL REQUIRED)
find_package(nlohmann_json QUIET)

if(NOT nlohmann_json_FOUND)
    message(FATAL_ERROR "nlohmann_json not found. TODO: add via FetchContent if unavailable on target system.")
endif()

add_executable(FortuneWheelBot
    src/main.cpp
    src/http/HttpClient.cpp
    src/telegram/TelegramClient.cpp
    src/telegram/TelegramUpdateParser.cpp
    src/domain/Wheel.cpp
    src/domain/WheelManager.cpp
    src/storage/InMemoryStorage.cpp
    src/bot/BotController.cpp
    src/bot/SpinAnimationService.cpp
)

target_include_directories(FortuneWheelBot PRIVATE src)

target_link_libraries(FortuneWheelBot PRIVATE
    Boost::system
    OpenSSL::SSL
    OpenSSL::Crypto
    nlohmann_json::nlohmann_json
)
