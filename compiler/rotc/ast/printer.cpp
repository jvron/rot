#include <variant>
#include <iostream>

#include "printer.hpp"
#include "ast/ast.hpp"
#include "ast/expr.hpp"
#include "ast/stmt.hpp"

struct Printer::ExprPrinter {

    Printer& printer;
    int indent = 0;

    ExprPrinter(Printer& printer, int indent) : printer(printer), indent(indent) {}

    void operator()(const BinaryExpr& expr) {
        printer.print_indent(indent);

        std::cout << "Binary expr(" << printer.get_lexeme(expr.op) << ")" << "\n";

        printer.print_expr(*expr.left, indent + 1);
        printer.print_expr(*expr.right, indent + 1);
    }

    void operator()(const UnaryExpr& expr) {
        printer.print_indent(indent);

        std::cout << "Unary expr(" << printer.get_lexeme(expr.op) << ")" << "\n";

        printer.print_expr(*expr.right, indent + 1);
    }

    void operator()(const LiteralExpr& expr) {
        printer.print_indent(indent);

        std::cout << "Literal expr(" << printer.get_lexeme(expr.literal) << ")" << "\n";
    }

    void operator()(const IdentifierExpr& expr) {
        printer.print_indent(indent);

        std::cout << "Identifier expr(" << printer.get_lexeme(expr.name) << ")" << "\n";
    }

    void operator()(const GroupingExpr& expr) {
        printer.print_indent(indent);

        std::cout << "Grouping expr: " << "\n";

        printer.print_expr(*expr.expr, indent + 1);
    }
}; 

struct Printer::StmtPrinter {

    Printer& printer;
    int indent = 0;

    StmtPrinter(Printer& printer, int indent) : printer(printer), indent(indent){}

    void operator()(const VarDeclarationStmt& stmt) {
        printer.print_indent(indent);

        std::cout << "variable declaration("<< printer.get_lexeme(stmt.var_name) << ")\n";
        printer.print_expr(*stmt.initializer, indent + 1);
    }

    void operator()(const AssignmentStmt& stmt) {
        printer.print_indent(indent);

        std::cout << "variable assignment("<< printer.get_lexeme(stmt.var_name) << ")\n";
        printer.print_expr(*stmt.initializer, indent + 1);
    }

    void operator()(const ExprStmt& stmt) {
        printer.print_indent(indent);

        printer.print_expr(*stmt.expr, indent + 1);
    }
};

void Printer::print_indent(int indent) {
    for (int i = 0; i < indent; i++) {
        std::cout << "    ";
    } 
}

std::string Printer::get_lexeme(const Token& token) {
    return source.substr(token.span.start.offset, token.span.end.offset - token.span.start.offset);
}

void Printer::print_expr(const Expr& expr, int indent) {
    ExprPrinter printer(*this, indent);
    std::visit(printer, expr.node);
}

void Printer::print_stmt(const Stmt &stmt, int indent) {
    StmtPrinter printer(*this, indent);
    std::visit(printer, stmt.node);
}

void Printer::print_ast(const Program& program) {

    for (const auto& statement : program.statements) {
        print_stmt(statement, 0);
    }
}