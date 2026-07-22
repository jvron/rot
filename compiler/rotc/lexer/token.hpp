#pragma once 

#include <cstdint>
#include <sys/types.h>

enum class TokenType {
    
    // punctuation
    LeftParen, RightParen,
    LeftBrace, RightBrace,
    Comma, Dot, SemiColon, Colon,

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

struct SourceLocation {
    uint32_t line {};
    uint32_t column {};
    uint32_t offset {}; // absolute position of the character 

    SourceLocation() = default;
    SourceLocation(uint32_t ln, uint32_t col, uint32_t offset) : line(ln), column(col), offset(offset) {}
};

struct SourceSpan {
    SourceLocation start;
    SourceLocation end;
};

struct Token {
    TokenType type;
    SourceSpan span;

    Token() = default;
    Token(TokenType type, SourceSpan& span) : type(type), span(span) {}
};