#pragma once

#include "expr.hpp"
#include "lexer/token.hpp"
#include <memory>
#include <variant>

struct ExprStmt {
    std::unique_ptr<Expr> expr;

    ExprStmt() = default;
    ExprStmt(Expr expression) : expr(std::make_unique<Expr>(std::move(expression))) {}
};

struct VarDeclarationStmt {
    Token var_name;
    std::unique_ptr<Expr> initializer;

    VarDeclarationStmt() = default;
    VarDeclarationStmt(Token name, Expr init) : var_name(std::move(name)), initializer(std::make_unique<Expr>(std::move(init))) {}
};

struct Stmt {
    std::variant<ExprStmt, VarDeclarationStmt> node;
};