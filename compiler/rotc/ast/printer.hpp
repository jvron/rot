#pragma once 

#include "ast/expr.hpp"
#include "parser/parser.hpp"

namespace Printer {
    void print_expr(const Expr& expr);
    void print_expr_tree(const ExprTree& tree);
}