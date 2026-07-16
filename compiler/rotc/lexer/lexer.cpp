#include <string>
#include <vector>

#include "lexer.hpp"
#include "core/result.hpp"
#include "lexer/token.hpp"

bool Lexer::is_end() {
    return current_idx >= source.length();
}

char Lexer::advance() {
    char c = source[current_idx];
    current_idx++;

    return c;
} 

char Lexer::peek() {
    if (is_end()) {
        return '\0';
    }

    return source[current_idx];
}

bool Lexer::match(char expected) {

    if (is_end()) {
        return false;
    }
    if(source[current_idx] !=  expected) {
        return false;
    }
    current_idx++;

    return false;
}

bool Lexer::scan_string() {

    while (peek() != '"' && !is_end()) {

        if (peek() == '\n') {
            line++;
        }
        current_idx++;
    }

    if (is_end()) {
        return false;
    }

    current_idx++; // consume trailing '"' 

    add_token(TokenType::String);
    return true;
}

void Lexer::add_token(TokenType type) {
    
    std::string lexeme = source.substr(start_idx, current_idx - start_idx);

    tokens.emplace_back(type, lexeme, line);
}

Result<std::vector<Token>> Lexer::scan_tokens() {

    while(!is_end()) {

        start_idx = current_idx;

        char c = advance();

        switch (c) {
            case '(': add_token(TokenType::LeftBrace); break;
            case ')': add_token(TokenType::RightBrace); break;
            case '{': add_token(TokenType::LeftParen); break;
            case '}': add_token(TokenType::RightParen); break;
            case ',': add_token(TokenType::Comma); break;
            case '.': add_token(TokenType::Dot); break;
            case '-': add_token(TokenType::Minus); break;
            case '+': add_token(TokenType::Plus); break;
            case '*': add_token(TokenType::Star); break;
            case ';': add_token(TokenType::SemiColon); break;

            case '!':
                add_token(match('=') ? TokenType::BangEqual : TokenType::Bang);
                break;
            case '=':
                add_token(match('=') ? TokenType::EqualEqual : TokenType::Eqaul);
                break;
            case '<':
                add_token(match('=') ? TokenType::LessEqual : TokenType::Less);
                break;
            case '>':
                add_token(match('=') ? TokenType::GreaterEqual : TokenType::Greater);
                break;

            case '/':
                if (match('/')) {
                    while (peek() != '\n' && !is_end()) {
                        current_idx++; // ignore till end of the comment
                    }
                }
                else {
                    add_token(TokenType::Slash);
                }
                break;
                
            case '"': 
                if (!scan_string()) {
                    std::string msg =  "At line " + std::to_string(line) + ": Unterminated string.";
                    return Result<std::vector<Token>>::failure(msg);
                }
                break;

            case '\n': line++; break;

            case ' ':
            case '\r':
            case '\t':
                break;

            default:
                std::string msg = "At line " + std::to_string(line) + ": Unexpected character.";
                return Result<std::vector<Token>>::failure(Error(msg));
        }
    }

    return Result<std::vector<Token>>::success(tokens);
}