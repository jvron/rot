#pragma once 

enum class ExitCode : int {
    Success = 0,
    UsageError = 64,
    FileError = 66,
    CompileError = 65,
    RuntimeError = 70,
};

constexpr int to_int(ExitCode exit_code) {
    return static_cast<int>(exit_code);
}