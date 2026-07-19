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
    bool check(TokenType expected);

    bool is_literal(const Token& token);

    Result<Expr> parse_expression();
    Result<Expr> parse_additive();
    Result<Expr> parse_multiplicative();
    Result<Expr> parse_unary();
    Result<Expr> parse_primary();
    Result<Expr> parse_literal();

public:
    Parser(const std::vector<Token>& tokens) : tokens(tokens) {};

    Result<ExprTree> parse_tokens();

};