#include <variant>
#include <iostream>

#include "printer.hpp"
#include "ast/ast.hpp"
#include "ast/expr.hpp"
#include "ast/stmt.hpp"

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

    void operator()(const IdentifierExpr& expr) {
        Printer::print_indent(indent);

        std::cout << "Identifier expr(" << expr.name.lexeme  << ")" << "\n";
    }

    void operator()(const GroupingExpr& expr) {
        Printer::print_indent(indent);

        std::cout << "Grouping expr: " << "\n";

        Printer::print_expr(*expr.expr, indent + 1);
    }
};

struct StmtPrinter {
    int indent = 0;
    StmtPrinter(int indent) : indent(indent) {};

    void operator()(const VarDeclarationStmt& stmt) {
        Printer::print_indent(indent);

        std::cout << "variable declaration("<< stmt.var_name.lexeme << ")\n";
        Printer::print_expr(*stmt.initializer, indent + 1);
    }

    void operator()(const ExprStmt& stmt) {
        Printer::print_indent(indent);

        Printer::print_expr(*stmt.expr, indent + 1);
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

void Printer::print_stmt(const Stmt &stmt, int indent) {
    StmtPrinter printer(indent);
    std::visit(printer, stmt.node);
}

void Printer::print_ast(const Program& program) {

    for (const auto& statement : program.statements) {
        print_stmt(statement, 0);
    }
}