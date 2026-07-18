#pragma once

#include <cstdint>
#include <vector>

#include "ast/expr.hpp"
#include "lexer/token.hpp"

struct ExprTree {
    Expr root;
};

class Parser {

private:
    const std::vector<Token>& tokens;
    uint32_t current_idx {};

    const Token& peek();
    bool is_end();
    void advance();

    Expr primary();
    Expr expression();
    Expr addition();

public:
    Parser(const std::vector<Token>& tokens) : tokens(tokens) {};

    ExprTree parse_tokens();

};