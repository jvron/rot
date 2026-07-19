#pragma once

#include <cstdint>
#include <vector>

#include "ast/expr.hpp"
#include "lexer/token.hpp"
#include "core/result.hpp"

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

    Result<Expr> expression();
    Result<Expr> additive();
    Result<Expr> multiplicative();
    Result<Expr> primary();

public:
    Parser(const std::vector<Token>& tokens) : tokens(tokens) {};

    Result<ExprTree> parse_tokens();

};