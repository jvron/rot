#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "core/result.hpp"
#include "lexer/token.hpp"

class Lexer {

private:
    const std::string& source;
    std::vector<Token> tokens;

    uint32_t start_idx {};
    uint32_t current_idx {};
    int line {1};

    bool is_end();
    bool is_digit(char value);
    bool is_alpha(char value);
    bool is_alphanumeric(char value);

    void advance(); // consume the character 
    char peek(); // get the next character without consuming 
    char peek_next(); 
    bool match(char expected); // check the current character and consume if it is equal to expected 
    bool scan_string();
    bool scan_number();
    void scan_identifier();
    void add_token(TokenType type);

public:
    Lexer(const std::string& src) : source(src) {}; 

    Result<std::vector<Token>> scan_tokens();
};