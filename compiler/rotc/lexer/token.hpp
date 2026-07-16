#pragma once 

#include <string>

enum class TokenType {
    // single character
    LeftParen, RightParen, LeftBrace, RightBrace,
    Comma, Dot, Minus, Plus, SemiColon, Slash, Star,  

    // one - two characters
    Equal, EqualEqual, 
    Bang, BangEqual, 
    Greater, GreaterEqual,
    Less, LessEqual,

    // literal 
    Identifier, String, Integer, Float, Character, 

    // keywords
    If, Else, 
    While, For,
    True, False,
    And, Or,
    Null, 
    Return, 
    Function,
    Print,  

    Eof 
};

struct Token {
    TokenType type;
    std::string lexeme; 
    int line {};

    Token() {};
    Token(TokenType token_type, std::string token_lexeme, int token_line)
        : type(token_type), lexeme(std::move(token_lexeme)), line(token_line) {};
};