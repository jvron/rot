
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

bool Parser::check(TokenType expected) {
    return peek().type == expected;
}

bool Parser::is_literal(const Token& token) {

    return  token.type == TokenType::Integer || 
            token.type == TokenType::True  ||
            token.type == TokenType::False;
}

bool Parser::is_comparison(const Token& token) {

    return  token.type == TokenType::Greater || 
            token.type == TokenType::Less ||
            token.type == TokenType::GreaterEqual ||
            token.type == TokenType::LessEqual;
}

Result<Expr> Parser::parse_literal() {

    const Token& token = peek(); 

    LiteralExpr literalExpr;
    literalExpr.literal = token;
            
    advance(); // consume literal

    Expr expr;
    expr.node = std::move(literalExpr);

    return Result<Expr>::success(std::move(expr));
}

Result<Expr> Parser::parse_primary() {

    if (is_literal(peek())) {
        return parse_literal();
    }
    else if (check(TokenType::LeftParen)) {

        advance(); // consume '('
        
        Result<Expr> inner = parse_expression();

        if (!inner) {
            return inner;
        }

        GroupingExpr groupingExpr;
        groupingExpr.expr = std::make_unique<Expr>(std::move(inner.value));

        if (peek().type != TokenType::RightParen) {
            std::string msg = "line: " + std::to_string(peek().line) + " Expected ')' after expression."; 
            return Result<Expr>::failure(Error(msg));
        }

        advance(); // consume ')'

        Expr expr;
        expr.node = std::move(groupingExpr);

        return Result<Expr>::success(std::move(expr));
    }
    else if (check(TokenType::Identifier)) {
        
        IdentifierExpr identifierExpr;
        identifierExpr.name = peek();

        advance(); // consume identifier

        Expr expr;
        expr.node = std::move(identifierExpr);

        return Result<Expr>::success(std::move(expr));
    }

    std::string msg = "line: " + std::to_string(peek().line) + " Expected expression, found: " + peek().lexeme; 
    return Result<Expr>::failure(Error(msg));
}

Result<Expr> Parser::parse_unary() {

    if (check(TokenType::Bang) || check(TokenType::Minus)) {
          
        UnaryExpr unaryExpr;
        unaryExpr.op = peek();

        advance(); // consume op

        Result<Expr> right = parse_unary();

        if (!right) {
            return right;
        }

        unaryExpr.right = std::make_unique<Expr>(std::move(right.value));

        Expr expr;
        expr.node = std::move(unaryExpr);

        return Result<Expr>::success(std::move(expr));
    }

    return parse_primary();
}

Result<Expr> Parser::parse_multiplicative() {

    Result<Expr> left = parse_unary();

    if (!left) {
        return left;
    }

    while (check(TokenType::Star) || check(TokenType::Slash)) {

        Token op = peek();
        advance(); // consume operator

        Result<Expr> right = parse_unary();

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

Result<Expr> Parser::parse_additive() {

    Result<Expr> left = parse_multiplicative();

    if (!left) {
        return left;
    }

    while (check(TokenType::Plus) ||check(TokenType::Minus)) {

        Token op = peek();
        advance(); // consume operator

        Result<Expr> right = parse_multiplicative();

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

Result<Expr> Parser::parse_comparison() {

    Result<Expr> left = parse_additive();

    if (!left) {
        return left;
    }

    while (is_comparison(peek())) {

        Token op = peek();
        advance();

        Result<Expr> right = parse_additive();

        if (!right) {
            return right;
        }

        BinaryExpr binaryExpr;
        binaryExpr.left = std::make_unique<Expr>(std::move(left.value));
        binaryExpr.op = op;
        binaryExpr.right = std::make_unique<Expr>(std::move(right.value));

        Expr expr;
        expr.node = std::move(binaryExpr);

        left.value = std::move(expr);
    }

    return Result<Expr>::success(std::move(left.value));
}

Result<Expr> Parser::parse_equality() {

    Result<Expr> left = parse_comparison();

    if (!left) {
        return left;
    }

    while (check(TokenType::EqualEqual) || check(TokenType::BangEqual)) {

        Token op = peek();
        advance();

        Result<Expr> right = parse_comparison();

        if (!right) {
            return right;
        }

        BinaryExpr binaryExpr;
        binaryExpr.left = std::make_unique<Expr>(std::move(left.value));
        binaryExpr.op = op;
        binaryExpr.right = std::make_unique<Expr>(std::move(right.value));

        Expr expr;
        expr.node = std::move(binaryExpr);

        left.value = std::move(expr);
    }

    return Result<Expr>::success(std::move(left.value));
}

Result<Expr> Parser::parse_logical_and() {

    Result<Expr> left = parse_equality();

    if (!left) {
        return left;
    }

    while (check(TokenType::LogicalAnd)) {

        Token op = peek();
        advance();

        Result<Expr> right = parse_equality();

        if (!right) {
            return right;
        }

        BinaryExpr binaryExpr;
        binaryExpr.left = std::make_unique<Expr>(std::move(left.value));
        binaryExpr.op = op;
        binaryExpr.right = std::make_unique<Expr>(std::move(right.value));

        Expr expr;
        expr.node = std::move(binaryExpr);

        left.value = std::move(expr);
    }

    return Result<Expr>::success(std::move(left.value));

}

Result<Expr> Parser::parse_logical_or() {

    Result<Expr> left = parse_logical_and();

    if (!left) {
        return left;
    }

    while (check(TokenType::LogicalOr)) {

        Token op = peek();
        advance();

        Result<Expr> right = parse_logical_and();

        if (!right) {
            return right;
        }

        BinaryExpr binaryExpr;
        binaryExpr.left = std::make_unique<Expr>(std::move(left.value));
        binaryExpr.op = op;
        binaryExpr.right = std::make_unique<Expr>(std::move(right.value));

        Expr expr;
        expr.node = std::move(binaryExpr);

        left.value = std::move(expr);
    }

    return Result<Expr>::success(std::move(left.value));

}

Result<Expr> Parser::parse_expression() {

/*  
    increasing precedence downwards

    expression = logical_or;

    logical_or = logical_and, { "||", logical_and };

    logical_and = comparison, { "&&", comparison};

    equality = comparison, { ("==" | "!="), comparison };

    comparison = additive, { ( "<" | "<=" | ">" | ">=" ), additive};

    additive = multiplicative, { ("+" | "-"), multiplicative };

    multiplicative = unary, { ("*" | "/"), unary};

    unary = ("!" | "-") unary | primary;

    primary = literal | IDENTIFIER |  "(" expression ")" ;

    literal = NUMBER | BOOLEAN 

*/
    return parse_logical_or();
}

Result<ExprTree> Parser::parse_tokens() {

    ExprTree exprTree;

    Result<Expr> expr = parse_expression();

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