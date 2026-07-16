#pragma once 

#include <string>

enum class TokenType {
    
    // punctuation
    LeftParen, RightParen,
    LeftBrace, RightBrace,
    Comma, Dot, SemiColon,

    // operators 
    Plus, Minus, Star, Slash,
    Equal, EqualEqual, 
    Bang, BangEqual, 
    Greater, GreaterEqual,
    Less, LessEqual,
    LogicalAnd, LogicalOr,

    // literals 
    Identifier, String, Integer, Float, Character, 

    // keywords
    If, Else, 
    While, For,
    True, False,
    Null, 
    Return, 
    Fn,
    Let,  

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