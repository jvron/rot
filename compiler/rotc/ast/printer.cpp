#include <variant>
#include <iostream>

#include "printer.hpp"
#include "ast/expr.hpp"
#include "parser/parser.hpp"

struct ExprPrinter {

    int indent = 0;

    ExprPrinter(int indent) : indent(indent) {};

    void operator()(const BinaryExpr& expr) {
        Printer::print_indent(indent);

        std::cout << "Binary expr(" << expr.op.lexeme << ")" << "\n";

        Printer::print_expr(*expr.left, indent + 1);
        Printer::print_expr(*expr.right, indent + 1);
    }

    void operator()(const UnaryExpr& expr) {
        Printer::print_indent(indent);

        std::cout << "Unary expr(" << expr.op.lexeme << ")" << "\n";

        Printer::print_expr(*expr.right, indent + 1);
    }

    void operator()(const LiteralExpr& expr) {
        Printer::print_indent(indent);

        std::cout << "Literal expr(" << expr.literal.lexeme  << ")" << "\n";
    }

    void operator()(const GroupingExpr& expr) {
        Printer::print_indent(indent);

        std::cout << "Grouping expr: " << "\n";

        Printer::print_expr(*expr.expr, indent + 1);
    }
};

void Printer::print_indent(int indent) {
    for (int i = 0; i < indent; i++) {
        std::cout << "    ";
    } 
}

void Printer::print_expr(const Expr& expr, int indent) {
    ExprPrinter printer(indent);
    std::visit(printer, expr.node);
}

void Printer::print_expr_tree(const ExprTree &tree) {
    print_expr(tree.root, 0);
}