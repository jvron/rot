#pragma once

#include <string>

struct Error {
    std::string message;

    Error() {};
    Error(std::string msg) : message(std::move(msg)) {}
};