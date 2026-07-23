#include <iostream>
#include <string>
#include <vector>

#include "ast/ast.hpp"
#include "ast/printer.hpp"
#include "core/exit_code.hpp"
#include "core/result.hpp"
#include "lexer/lexer.hpp"
#include "parser/parser.hpp"
#include "util/file.hpp"

int main(int argc, char* argv[]) {

    if (argc < 2) {
        std::cerr << "Usage: rotc <file> \n";
        return to_int(ExitCode::UsageError);
    }

    std::string path = argv[1];
    Result<std::string> source = File::read(path);

    if (!source) {
        std::cerr << source.error.message << "\n";
        return to_int(ExitCode::FileError);
    }

    Lexer lexer(source.value);
    
    Result<std::vector<Token>> tokens =  lexer.scan_tokens();

    if (!tokens) {
        std::cerr << tokens.error.message << "\n";
        return to_int(ExitCode::CompileError);
    }

    Parser parser(tokens.value, source.value);
    Result<Program> program =  parser.parse_program();

    if (!program) {
        std::cerr << program.error.message << "\n";
        return to_int(ExitCode::CompileError);
    }

    Printer printer(source.value);
    printer.print_ast(program.value);
    
    return to_int(ExitCode::Success); 
}