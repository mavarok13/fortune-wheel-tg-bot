#pragma once

#include <stdexcept>
#include <string>

class WheelNotSelectedException : public std::runtime_error {
public:
    explicit WheelNotSelectedException(const std::string& message) : std::runtime_error(message) {}
};

class WheelNotFoundException : public std::runtime_error {
public:
    explicit WheelNotFoundException(const std::string& message) : std::runtime_error(message) {}
};

class WheelItemNotFoundException : public std::runtime_error {
public:
    explicit WheelItemNotFoundException(const std::string& message) : std::runtime_error(message) {}
};

class InvalidWheelOperationException : public std::runtime_error {
public:
    explicit InvalidWheelOperationException(const std::string& message) : std::runtime_error(message) {}
};
