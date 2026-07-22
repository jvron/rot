#include <string>
#include <unordered_map>
#include <vector>

#include "lexer.hpp"
#include "core/result.hpp"
#include "lexer/token.hpp"

static const std::unordered_map<std::string, TokenType> keywords = {
    {"if", TokenType::If},
    {"else", TokenType::Else},
    {"while", TokenType::While},
    {"for", TokenType::For},
    {"true", TokenType::True},
    {"false", TokenType::False},
    {"null", TokenType::Null},
    {"return", TokenType::Return},
    {"fn", TokenType::Fn},
    {"let", TokenType::Let},
};

bool Lexer::is_end() {
    return current_idx >= source.length();
}

bool Lexer::is_digit(char value) {
    return value >= '0' && value <= '9';
}

bool Lexer::is_alpha(char value) {
    return  (value >= 'a' && value <= 'z') ||
            (value >= 'A' && value <= 'Z' ) ||
            value == '_';
}

bool Lexer::is_alphanumeric(char value) {
    return is_alpha(value) || is_digit(value);
}

void Lexer::advance() {
    column++;
    current_idx++;
}

char Lexer::peek() {
    if (is_end()) {
        return '\0';
    }

    return source[current_idx];
}

char Lexer::peek_next() {
    if (current_idx + 1 >= source.length()) {
        return '\0';
    }

    return source[current_idx + 1];
}

bool Lexer::match(char expected) {

    if (is_end()) {
        return false;
    }

    if(source[current_idx] != expected) {
        return false;
    }

    advance();
    return true;
}

bool Lexer::scan_string() {

    while (peek() != '"' && !is_end()) {

        if (peek() == '\n') {
            line++;
            column = 1;
        }
        advance();
    }

    if (is_end()) {
        return false; // unterminated string
    }

    advance(); // consume trailing '"' 

    add_token(TokenType::String);
    return true;
}

bool Lexer::scan_number() {
    
    while (is_digit(peek()) && !is_end()) {
        advance();
    }

    if (peek() == '.' && is_digit(peek_next())) {
        advance(); // consume '.'

        while (is_digit(peek())) {
            advance();
        }
        add_token(TokenType::Float);
        return true;
    }

    add_token(TokenType::Integer);
    return true;
}

void Lexer::scan_identifier() {

    while (!is_end() && is_alphanumeric(peek())) {
        advance();
    }
    std::string text = source.substr(start_idx, current_idx - start_idx);

    auto it = keywords.find(text);

    if (it != keywords.end()) {
        TokenType type = it->second;
        add_token(type);
    }
    else {
        add_token(TokenType::Identifier);
    }
}

void Lexer::add_token(TokenType type) {
    
    SourceSpan span = {
        .start = SourceLocation(line, column, start_idx),
        .end = SourceLocation(line, column, current_idx - start_idx),
    };

    tokens.emplace_back(type, span);
}

Result<std::vector<Token>> Lexer::scan_tokens() {

    while(!is_end()) {

        start_idx = current_idx;

        char c = peek();
        advance();

        switch (c) {
            case '(': add_token(TokenType::LeftParen); break;
            case ')': add_token(TokenType::RightParen); break;
            case '{': add_token(TokenType::LeftBrace); break;
            case '}': add_token(TokenType::RightBrace); break;
            case ',': add_token(TokenType::Comma); break;
            case '.': add_token(TokenType::Dot); break;
            case '-': add_token(TokenType::Minus); break;
            case '+': add_token(TokenType::Plus); break;
            case '*': add_token(TokenType::Star); break;
            case ';': add_token(TokenType::SemiColon); break;
            case ':': add_token(TokenType::Colon); break;

            case '!':
                add_token(match('=') ? TokenType::BangEqual : TokenType::Bang);
                break;
            case '=':
                add_token(match('=') ? TokenType::EqualEqual : TokenType::Equal);
                break;
            case '<':
                add_token(match('=') ? TokenType::LessEqual : TokenType::Less);
                break;
            case '>':
                add_token(match('=') ? TokenType::GreaterEqual : TokenType::Greater);
                break;
            case '&':
                if (match('&')) {
                    add_token(TokenType::LogicalAnd);
                }
                break;
            case '|':
                if (match('|')) {
                    add_token(TokenType::LogicalOr);
                }
                break;

            case '/':
                if (match('/')) {
                    while (peek() != '\n' && !is_end()) {
                        advance(); // ignore till end of the comment
                    }   
                }
                else if (match('*')) {

                    auto is_closing = [this](){
                        return peek() == '*' && peek_next() == '/';
                    };

                    while (!is_closing() && !is_end()) {
                        if (peek() == '\n') {
                            line++;
                            column = 1;
                        }
                        advance();
                    }
                    if (!is_closing()) {
                        std::string msg =  "line " + std::to_string(line) + ": Unterminated comment block.";
                        return Result<std::vector<Token>>::failure(msg);
                    }
                    // consume trailing "*/"
                    advance();
                    advance(); 
                }
                else {
                    add_token(TokenType::Slash);
                }
                break;
                
            case '"': 
                if (!scan_string()) {
                    std::string msg =  "line " + std::to_string(line) + ": Unterminated string.";
                    return Result<std::vector<Token>>::failure(msg);
                }
                break;
            case '\'':
                if (is_end()) {
                    std::string msg =  "line " + std::to_string(line) + ": Missing terminating ' character.";
                    return Result<std::vector<Token>>::failure(msg);
                }
                advance(); // consume character

                if (peek() != '\'') {
                    std::string msg =  "line " + std::to_string(line) + ": Missing terminating ' character.";
                    return Result<std::vector<Token>>::failure(msg);                
                }

                advance(); // consume trailing ' 
                add_token(TokenType::Character);
                break;

            case '\n': 
                line++;
                column = 1; 
                break;

            case ' ':
            case '\r':
            case '\t':
                break; 
                
            default:
                if (is_digit(c)) {
                    scan_number();
                }
                else if (is_alpha(c)) {
                    scan_identifier();
                }
                else {
                    std::string msg = "line " + std::to_string(line) + ": Unexpected character.";
                    return Result<std::vector<Token>>::failure(Error(msg));
                }
        }
    }

    SourceSpan span = {
        .start = SourceLocation(line, column, source.size()),
        .end = SourceLocation(line, column, source.size()),
    };
    tokens.emplace_back(TokenType::Eof, span);

    return Result<std::vector<Token>>::success(tokens);
}