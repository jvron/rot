#include <iostream>
#include <string>
#include <vector>

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
    Result<std::string> result = File::read(path);

    if (!result) {
        std::cerr << result.error.message << "\n";
        return to_int(ExitCode::FileError);
    }

    Lexer lexer(result.value);
    
    Result<std::vector<Token>> tokens =  lexer.scan_tokens();

    if (!tokens) {
        std::cerr << tokens.error.message << "\n";
        return to_int(ExitCode::CompileError);
    }

    Parser parser(tokens.value);
    Result<ExprTree> tree =  parser.parse_tokens();

    if (!tree) {
        std::cerr << tree.error.message << "\n";
        return to_int(ExitCode::CompileError);
    }

    Printer::print_expr_tree(tree.value);
    
    return to_int(ExitCode::Success); 
}