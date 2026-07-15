#include <iostream>

#include "core/exit_code.hpp"
#include "core/result.hpp"
#include "util/file.hpp"

int main(int argc, char* argv[]) {

    if (argc < 2) {
        std::cerr << "Usage: rotc <file> \n";
        return to_int(ExitCode::UsageError);
    }

    std::string path = argv[1];
    Result<std::string> result = File::read(path);

    if (!result) {
        std::cerr << result.error.message << "\n";
        return to_int(ExitCode::FileError);
    }

    return to_int(ExitCode::Success); 
}