#pragma once 

#include "ast/ast.hpp"
#include "ast/expr.hpp"
#include "ast/stmt.hpp"
#include "parser/parser.hpp"

namespace Printer {
    void print_indent(int indent);
    void print_expr(const Expr& expr, int indent);
    void print_stmt(const Stmt& stmt, int indent);
    void print_ast(const Program& program);
}