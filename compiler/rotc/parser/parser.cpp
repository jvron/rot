
#include <memory>
#include <variant>

#include "parser.hpp"
#include "ast/expr.hpp"
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

Expr Parser::primary() {

    if (peek().type == TokenType::Integer) {

        Token numberToken = peek();

        advance(); // consume literal 

        LiteralExpr literalExpr;
        literalExpr.literal = numberToken;

        Expr expr;
        expr.node = std::move(literalExpr);

        return expr;
    }

    if (peek().type == TokenType::LeftParen) {

        advance(); // consume '('
        
        Expr inner = expression();

        GroupingExpr groupingExpr;
        groupingExpr.expr = std::make_unique<Expr>(std::move(inner));

        if (peek().type == TokenType::RightParen) {
            advance(); // consume ')'
        }

        Expr expr;
        expr.node = std::move(groupingExpr);

        return expr;
    }

    return {};
}

Expr Parser::multiplicative() {

    Expr left = primary();

    while (peek().type == TokenType::Star || peek().type == TokenType::Slash) {

        Token op = peek();
        advance(); // consume operator

        Expr right = primary();

        BinaryExpr binaryExpr;
        binaryExpr.left = std::make_unique<Expr>(std::move(left));
        binaryExpr.op = op;
        binaryExpr.right = std::make_unique<Expr>(std::move(right));

        Expr expr;
        expr.node = std::move(binaryExpr);
        
        left = std::move(expr); // grow the tree
    }

    return left;
}

Expr Parser::additive() {

    Expr left = multiplicative();

    while (peek().type == TokenType::Plus || peek().type == TokenType::Minus) {

        Token op = peek();
        advance(); // consume operator

        Expr right = multiplicative();

        BinaryExpr binaryExpr;
        binaryExpr.left = std::make_unique<Expr>(std::move(left));
        binaryExpr.op = op;
        binaryExpr.right = std::make_unique<Expr>(std::move(right));

        Expr expr;
        expr.node = std::move(binaryExpr); // move because binaryExpr contains unique pointers (it cannot be copied)
        
        left = std::move(expr); // grow the tree
    }

    return left;
}

Expr Parser::expression() {

/*
    expression = additive ;

    additive = multiplicative, { ("+" | "-"), multiplicative };

    multiplicative = primary, { ("*" | "/"), primary };

    primary = NUMBER | "(" expression ")" ;

*/

    return additive();
}

ExprTree Parser::parse_tokens() {

    ExprTree exprTree;
    exprTree.root = expression();
    return exprTree;
}