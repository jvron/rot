#pragma once 

#include <memory>
#include <variant>

#include "lexer/token.hpp"

struct Expr;

struct BinaryExpr {
    std::unique_ptr<Expr> left;
    Token op;
    std::unique_ptr<Expr> right;
};

struct UnaryExpr {
    Token op;
    std::unique_ptr<Expr> right;
};

struct LiteralExpr {
    Token literal;
};

struct IdentifierExpr {
    Token name;
};

struct GroupingExpr {
    std::unique_ptr<Expr> expr;
};

struct Expr {
    std::variant<
        BinaryExpr, UnaryExpr, 
        LiteralExpr, IdentifierExpr, 
        GroupingExpr
        > node;
};
