#ifndef QUASTRA_LEXER_H
#define QUASTRA_LEXER_H

#include "common.h"

// All keywords from the Quastra spec are now represented.
typedef enum {
    TOKEN_EOF,
    TOKEN_IDENTIFIER,
    TOKEN_INT_LITERAL,
    TOKEN_FLOAT_LITERAL,
    TOKEN_STRING_LITERAL,
    
    // Keywords
    TOKEN_KEYWORD_FN,
    TOKEN_KEYWORD_LET,
    TOKEN_KEYWORD_MUT,
    TOKEN_KEYWORD_RETURN,
    TOKEN_KEYWORD_RECORD,
    TOKEN_KEYWORD_TYPE,
    TOKEN_KEYWORD_PROTOCOL,
    TOKEN_KEYWORD_IMPL,
    TOKEN_KEYWORD_EXTEND,
    TOKEN_KEYWORD_PUB,
    TOKEN_KEYWORD_USE,
    TOKEN_KEYWORD_MODULE,
    TOKEN_KEYWORD_UNSAFE,
    TOKEN_KEYWORD_IF,
    TOKEN_KEYWORD_ELSE,
    TOKEN_KEYWORD_WHILE,
    TOKEN_KEYWORD_FOR,
    TOKEN_KEYWORD_IN,
    TOKEN_KEYWORD_YIELD,
    TOKEN_KEYWORD_MATCH,
    TOKEN_KEYWORD_TRY,
    TOKEN_KEYWORD_SPAWN,
    TOKEN_KEYWORD_AWAIT,
    TOKEN_KEYWORD_SCOPE,
    TOKEN_KEYWORD_USING,
    TOKEN_KEYWORD_CONST,
    
    // Separators
    TOKEN_LEFT_PAREN,
    TOKEN_RIGHT_PAREN,
    TOKEN_LEFT_BRACE,
    TOKEN_RIGHT_BRACE,
    TOKEN_LEFT_BRACKET,
    TOKEN_RIGHT_BRACKET,
    TOKEN_SEMICOLON,
    TOKEN_COLON,
    TOKEN_COMMA,
    TOKEN_DOT,

    // Operators
    TOKEN_ASSIGN, // =
    TOKEN_EQUAL, // ==
    TOKEN_BANG, // !
    TOKEN_BANG_EQUAL, // !=
    TOKEN_PLUS,
    TOKEN_MINUS,
    TOKEN_STAR, // *
    TOKEN_SLASH, // /
    TOKEN_LESS, // <
    TOKEN_LESS_EQUAL,
    TOKEN_GREATER, // >
    TOKEN_GREATER_EQUAL,
    TOKEN_PIPE, // |
    TOKEN_PIPE_PIPE, // ||
    TOKEN_AMPERSAND, // &
    TOKEN_AMPERSAND_AMPERSAND, // &&
    TOKEN_PLUS_ASSIGN, // +=
    TOKEN_MINUS_ASSIGN, // -=
    TOKEN_STAR_ASSIGN, // *=
    TOKEN_SLASH_ASSIGN, // /=
    TOKEN_ERROR, // For unrecognized tokens
} TokenType;

typedef struct {
    TokenType type;
    const char* lexeme;
    int length;
} Token;

typedef struct {
    const char* source;
    size_t start;
    size_t current;
} Lexer;

void quastra_lexer_init(Lexer* lexer, const char* source);
Token quastra_lexer_scan_token(Lexer* lexer);

#endif // QUASTRA_LEXER_H