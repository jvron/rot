#pragma once 

#include <string>

#include "core/result.hpp"

namespace File  {
    Result<std::string> read(const std::string& path);
}