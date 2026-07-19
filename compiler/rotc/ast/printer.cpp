#include <variant>
#include <iostream>

#include "printer.hpp"
#include "ast/expr.hpp"

struct ExprPrinter {

    void operator()(const BinaryExpr& expr) {

        std::cout << "Binary expr(" << expr.op.lexeme << ")" << "\n";

        Printer::print_expr(*expr.left);
        Printer::print_expr(*expr.right);
    }

    void operator()(const UnaryExpr& expr) {
        std::cout << "Unary expr(" << expr.op.lexeme << ")" << "\n";

        Printer::print_expr(*expr.right);
    }

    void operator()(const LiteralExpr& expr) {
        std::cout << "\tLiteral expr(" << expr.literal.lexeme  << ")" << "\n";
    }

    void operator()(const GroupingExpr& expr) {
        std::cout << "Grouping expr: " << "\n";

        Printer::print_expr(*expr.expr);
    }
};

void Printer::print_expr(const Expr& expr) {

    ExprPrinter printer;
    std::visit(printer, expr.node);
}

void Printer::print_expr_tree(const ExprTree &tree) {
    print_expr(tree.root);
}