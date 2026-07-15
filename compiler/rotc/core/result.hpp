#pragma once 

#include "error.hpp"

template <typename T>
class Result {

private:
    bool succeeded {false};

public:
    T value {};
    Error error;

    Result<T>(T value) : succeeded(true), value(std::move(value)) {}
    Result<T>(Error error) : succeeded(false), error(std::move(error)) {}

    static Result<T> success(T value) {
        return Result<T>(value);
    }

    static Result<T> failure(Error error) {
        return Result<T>(error);
    }

    // allow Result to be used in boolean contexts (if, !, &&, ||)
    // explicit conversion of Result to bool based on the succeeded state
    explicit operator bool() const {
        return succeeded;
    }
};