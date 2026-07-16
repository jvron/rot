#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "core/result.hpp"
#include "lexer/token.hpp"

class Lexer {

private:
    std::string source;
    std::vector<Token> tokens;

    uint32_t start_idx {};
    uint32_t current_idx {};
    int line {1};

    bool is_end();
    bool is_digit(char value);

    char advance(); // get and consume the current character 
    char peek(); // get the next character without consuming 
    char peek_next(); 
    bool match(char expected); // check the next character and consume if it is equal to expected 
    bool scan_string();
    bool scan_number();
    void add_token(TokenType type);

public:
    Lexer(std::string& src) : source(std::move(src)) {}; 

    Result<std::vector<Token>> scan_tokens();
};