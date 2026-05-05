#include "InMemoryStorage.hpp"

Wheel& InMemoryStorage::getWheel(int64_t chatId) {
    return wheels_[chatId];
}
