#include <cstdio>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "file.hpp"
#include "core/result.hpp"

Result<std::string> File::read(const std::string& path) {

    std::ifstream file(path);

    if (!file) {
        return Result<std::string>::failure(
            Error("Could not open file: " + path)
        );
    }

    std::stringstream buffer;
    buffer << file.rdbuf();

    return Result<std::string>::success(buffer.str());
}