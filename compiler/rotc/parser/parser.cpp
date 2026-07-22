/*  
    program = { statement };

    statement = assignment_statement | variable_declaration | expression;

    assignment_statement = IDENTIFER, "=", expression, ";"

    variable_declaration = "let", IDENTIFER, "=", expression, ";"

    expression_statement = expression, ";";

    expression = logical_or;

    logical_or = logical_and, { "||", logical_and };

    logical_and = comparison, { "&&", comparison};

    equality = comparison, { ("==" | "!="), comparison };

    comparison = additive, { ( "<" | "<=" | ">" | ">=" ), additive};

    additive = multiplicative, { ("+" | "-"), multiplicative };

    multiplicative = unary, { ("*" | "/"), unary};

    unary = ("!" | "-") unary | primary;

    primary = literal | IDENTIFIER |  "(" expression ")" ;

    literal = INTEGER | FLOAT | BOOLEAN | STRING | CHARACTER 

*/

#include <memory>
#include <string>
#include <utility>
#include <variant>

#include "parser.hpp"
#include "ast/ast.hpp"
#include "ast/expr.hpp"
#include "ast/stmt.hpp"
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

const Token& Parser::peek_next() {
    return tokens[current_idx + 1];
}

bool Parser::check(TokenType expected) {
    return peek().type == expected;
}

bool Parser::check_next(TokenType expected) {
    return peek_next().type == expected;
}

bool Parser::is_literal(const Token& token) {

    switch (token.type) {
        case TokenType::Integer:
        case TokenType::Float:
        case TokenType::String:
        case TokenType::Character:
        case TokenType::True:
        case TokenType::False:
            return true;

        default:
            return false;
    }
}

bool Parser::is_comparison(const Token& token) {

    switch (token.type) {
        case TokenType::Greater:
        case TokenType::Less:
        case TokenType::GreaterEqual:
        case TokenType::LessEqual:
            return true;

        default:
            return false;
    }
}

bool Parser::is_expression_start(const Token& token) {

    switch (token.type) {
        case TokenType::Integer:
        case TokenType::Float:
        case TokenType::String:
        case TokenType::Character:
        case TokenType::True:
        case TokenType::False:
        case TokenType::Identifier:
        case TokenType::LeftParen:
        case TokenType::Minus:
        case TokenType::Bang:
            return true;
        default:
            return false;
    }
}

Expr Parser::create_binary(Expr left, const Token& op, Expr right) {

    BinaryExpr binaryExpr;
    binaryExpr.left = std::make_unique<Expr>(std::move(left));
    binaryExpr.op = op;
    binaryExpr.right = std::make_unique<Expr>(std::move(right));

    Expr expr;
    expr.node = std::move(binaryExpr);

    return expr;
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
            std::string msg = "line " + std::to_string(peek().line) + ": Expected ')' after expression."; 
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

    std::string msg = "line " + std::to_string(peek().line) + ": Expected expression, found: " + peek().lexeme; 
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

        const Token& op = peek();
        advance(); // consume operator

        Result<Expr> right = parse_unary();

        if (!right) {
            return right;
        }
        
        left.value = create_binary(std::move(left.value), op, std::move(right.value));; // grow the tree
    }

    return Result<Expr>::success(std::move(left.value));
}

Result<Expr> Parser::parse_additive() {

    Result<Expr> left = parse_multiplicative();

    if (!left) {
        return left;
    }

    while (check(TokenType::Plus) ||check(TokenType::Minus)) {

        const Token& op = peek();
        advance(); // consume operator

        Result<Expr> right = parse_multiplicative();

        if (!right) {
            return right;
        }

        left.value = create_binary(std::move(left.value), op, std::move(right.value));
    }

    return Result<Expr>::success(std::move(left.value));
}

Result<Expr> Parser::parse_comparison() {

    Result<Expr> left = parse_additive();

    if (!left) {
        return left;
    }

    while (is_comparison(peek())) {

        const Token& op = peek();
        advance();

        Result<Expr> right = parse_additive();

        if (!right) {
            return right;
        }

        left.value = create_binary(std::move(left.value), op, std::move(right.value));
    }

    return Result<Expr>::success(std::move(left.value));
}

Result<Expr> Parser::parse_equality() {

    Result<Expr> left = parse_comparison();

    if (!left) {
        return left;
    }

    while (check(TokenType::EqualEqual) || check(TokenType::BangEqual)) {

        const Token& op = peek();
        advance();

        Result<Expr> right = parse_comparison();

        if (!right) {
            return right;
        }

        left.value = create_binary(std::move(left.value), op, std::move(right.value));
    }

    return Result<Expr>::success(std::move(left.value));
}

Result<Expr> Parser::parse_logical_and() {

    Result<Expr> left = parse_equality();

    if (!left) {
        return left;
    }

    while (check(TokenType::LogicalAnd)) {

        const Token& op = peek();
        advance();

        Result<Expr> right = parse_equality();

        if (!right) {
            return right;
        }

        left.value = create_binary(std::move(left.value), op, std::move(right.value));
    }

    return Result<Expr>::success(std::move(left.value));
}

Result<Expr> Parser::parse_logical_or() {

    Result<Expr> left = parse_logical_and();

    if (!left) {
        return left;
    }

    while (check(TokenType::LogicalOr)) {

        const Token& op = peek();
        advance();

        Result<Expr> right = parse_logical_and();

        if (!right) {
            return right;
        }

        left.value = create_binary(std::move(left.value), op, std::move(right.value));
    }

    return Result<Expr>::success(std::move(left.value));
}

Result<Expr> Parser::parse_expression() {
    return parse_logical_or();
}

Result<Stmt> Parser::parse_expression_statement() {
    Result<Expr> expression = parse_expression();

    if (!expression) {
        return Result<Stmt>::failure(expression.error);
    }

    if (!check(TokenType::SemiColon)) {
        std::string msg = "line " + std::to_string(peek().line) + ": Expected ';' after expression, found '" + peek().lexeme + "'."; 
        return Result<Stmt>::failure(Error(msg));
    }
    advance(); // consume ';'

    ExprStmt expr_stmt(std::move(expression.value));

    Stmt stmt = {
        .node = std::move(expr_stmt)
    };

    return Result<Stmt>::success(std::move(stmt));
}

Result<Stmt> Parser::parse_var_declaration() {

    advance(); // consume 'let'

    if (!check(TokenType::Identifier)) {
        std::string msg = "line " + std::to_string(peek().line) + ": Expected identifier after 'let', found '" + peek().lexeme + "'."; 
        return Result<Stmt>::failure(Error(msg));
    }
    Token var_name = peek(); 
    advance(); // consume identifier

    if (!check(TokenType::Equal)) {
        std::string msg = "line " + std::to_string(peek().line) + ": Expected '=' after variable name, found '" + peek().lexeme + "'."; 
        return Result<Stmt>::failure(Error(msg));
    }
    advance(); // consume '='

    Result<Expr> initializer = parse_expression();
    
    if (!initializer) {
        return Result<Stmt>::failure(initializer.error);
    }

    if (!check(TokenType::SemiColon)) {
        std::string msg = "line " + std::to_string(peek().line) + ": Expected ';' after expression, found '" + peek().lexeme + "'."; 
        return Result<Stmt>::failure(Error(msg));
    }
    advance(); // consume ';'

    VarDeclarationStmt var_decl_stmt(var_name, std::move(initializer.value));

    Stmt stmt = {
        .node = std::move(var_decl_stmt)
    };
    
    return Result<Stmt>::success(std::move(stmt));
}

Result<Stmt> Parser::parse_assignment() {

    Token var_name = peek();
    advance();

    if (!check(TokenType::Equal)) {
        std::string msg = "line " + std::to_string(peek().line) + ": Expected '=' after variable name, found '" + peek().lexeme + "'."; 
        return Result<Stmt>::failure(Error(msg));
    }
    advance();

    Result<Expr> value = parse_expression();

    if (!value) {
        return Result<Stmt>::failure(value.error); 
    }

    if (!check(TokenType::SemiColon)) {
        std::string msg = "line " + std::to_string(peek().line) + ": Expected ';' after expression, found '" + peek().lexeme + "'."; 
        return Result<Stmt>::failure(Error(msg));
    }
    advance(); 

    AssignmentStmt assignment_stmt(var_name, std::move(value.value));

    Stmt stmt = {
        .node = std::move(assignment_stmt)
    };

    return Result<Stmt>::success(std::move(stmt));
}

Result<Stmt> Parser::parse_statement() {

    if (check(TokenType::Let)) {
        return parse_var_declaration();
    }
    if (check(TokenType::Identifier) && check_next(TokenType::Equal)) {
        return parse_assignment();
    }
    if (is_expression_start(peek())) {
        return parse_expression_statement();
    }
    
    std::string msg = "line " + std::to_string(peek().line) +": Expected statement, found: " + peek().lexeme;
    return Result<Stmt>::failure(Error(msg));
}

Result<Program> Parser::parse_program() {

    Program program;

    while (!is_end()) {
        Result<Stmt> statement = parse_statement();
        
        if (!statement) {
            return Result<Program>::failure(statement.error);
        }
        program.statements.push_back(std::move(statement.value));
    }

    return Result<Program>::success(std::move(program));
}