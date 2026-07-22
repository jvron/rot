#pragma once

#include <cstdint>
#include <vector>

#include "ast/ast.hpp"
#include "ast/expr.hpp"
#include "lexer/token.hpp"
#include "core/result.hpp"
#include "ast/stmt.hpp"

class Parser {

private:
    const std::vector<Token>& tokens;
    uint32_t current_idx {};

    const Token& peek();
    bool is_end();
    void advance();
    bool check(TokenType expected);

    bool is_literal(const Token& token);
    bool is_comparison(const Token& token);
    bool is_expression_start(const Token& token);

    Expr create_binary(Expr left, const Token& op, Expr right);

    Result<Stmt> parse_statement();
    Result<Stmt> parse_var_declaration();
    Result<Stmt> parse_expression_statement();
    Result<Expr> parse_expression();
    Result<Expr> parse_logical_or();    
    Result<Expr> parse_logical_and();
    Result<Expr> parse_equality();
    Result<Expr> parse_comparison();
    Result<Expr> parse_additive();
    Result<Expr> parse_multiplicative();
    Result<Expr> parse_unary();
    Result<Expr> parse_primary();
    Result<Expr> parse_literal();

public:
    Parser(const std::vector<Token>& tokens) : tokens(tokens) {};

    Result<Program> parse_program();

};