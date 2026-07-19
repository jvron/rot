
#include <memory>
#include <string>
#include <variant>

#include "parser.hpp"
#include "ast/expr.hpp"
#include "core/error.hpp"
#include "core/result.hpp"
#include "lexer/token.hpp"

bool Parser::is_end() {
    return tokens[current_idx].type == TokenType::Eof;
}

void Parser::advance() {
    if (!is_end()) {
        current_idx++;
    }
}

const Token& Parser::peek() {
    return tokens[current_idx];
}

Result<Expr> Parser::primary() {

    if (peek().type == TokenType::Integer) {

        Token numberToken = peek();

        advance(); // consume literal 

        LiteralExpr literalExpr;
        literalExpr.literal = numberToken;

        Expr expr;
        expr.node = std::move(literalExpr);

        return Result<Expr>::success(std::move(expr));
    }

    if (peek().type == TokenType::LeftBrace) {

        advance(); // consume '('
        
        Result<Expr> inner = expression();

        if (!inner) {
            return inner;
        }

        GroupingExpr groupingExpr;
        groupingExpr.expr = std::make_unique<Expr>(std::move(inner.value));

        if (peek().type != TokenType::RightBrace) {
            std::string msg = "line: " + std::to_string(peek().line) + " Expected ')' after expression."; 
            return Result<Expr>::failure(Error(msg));
        }

        advance(); // consume ')'

        Expr expr;
        expr.node = std::move(groupingExpr);

        return Result<Expr>::success(std::move(expr));
    }
    std::string msg = "line: " + std::to_string(peek().line) + " Expected number or '(', found: " + peek().lexeme; 
    return Result<Expr>::failure(Error(msg));
}

Result<Expr> Parser::multiplicative() {

    Result<Expr> left = primary();

    if (!left) {
        return left;
    }

    while (peek().type == TokenType::Star || peek().type == TokenType::Slash) {

        Token op = peek();
        advance(); // consume operator

        Result<Expr> right = primary();

        if (!right) {
            return right;
        }

        BinaryExpr binaryExpr;
        binaryExpr.left = std::make_unique<Expr>(std::move(left.value));
        binaryExpr.op = op;
        binaryExpr.right = std::make_unique<Expr>(std::move(right.value));

        Expr expr;
        expr.node = std::move(binaryExpr);
        
        left.value = std::move(expr); // grow the tree
    }

    return Result<Expr>::success(std::move(left.value));
}

Result<Expr> Parser::additive() {

    Result<Expr> left = multiplicative();

    if (!left) {
        return left;
    }

    while (peek().type == TokenType::Plus || peek().type == TokenType::Minus) {

        Token op = peek();
        advance(); // consume operator

        Result<Expr> right = multiplicative();

        if (!right) {
            return right;
        }

        BinaryExpr binaryExpr;
        binaryExpr.left = std::make_unique<Expr>(std::move(left.value));
        binaryExpr.op = op;
        binaryExpr.right = std::make_unique<Expr>(std::move(right.value));

        Expr expr;
        expr.node = std::move(binaryExpr); // move because binaryExpr contains unique pointers (it cannot be copied)
        
        left.value = std::move(expr); // grow the tree
    }

    return Result<Expr>::success(std::move(left.value));
}

Result<Expr> Parser::expression() {

/*
    expression = additive ;

    additive = multiplicative, { ("+" | "-"), multiplicative };

    multiplicative = primary, { ("*" | "/"), primary };

    primary = NUMBER | "(" expression ")" ;

*/

    return additive();
}

Result<ExprTree> Parser::parse_tokens() {

    ExprTree exprTree;

    Result<Expr> expr = expression();

    if (!expr) {
        return Result<ExprTree>::failure(expr.error);
    }

    if (!is_end()) {
        std::string msg = "line: " + std::to_string(peek().line) + " Unexpected token: "+ peek().lexeme ; 
        return Result<ExprTree>::failure(Error(msg));
    }
    exprTree.root = std::move(expr.value);
    return Result<ExprTree>::success(std::move(exprTree));
}