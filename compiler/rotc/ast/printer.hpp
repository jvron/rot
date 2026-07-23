#pragma once 

#include "ast/ast.hpp"
#include "ast/expr.hpp"
#include "ast/stmt.hpp"


class Printer {

private:
    const std::string& source;

    struct ExprPrinter;
    struct StmtPrinter;

    void print_indent(int indent);
    std::string get_lexeme(const Token& token);

    void print_expr(const Expr& expr, int indent);
    void print_stmt(const Stmt& stmt, int indent);

public:
    Printer(const std::string& src) : source(src) {}

    void print_ast(const Program& program);
};
