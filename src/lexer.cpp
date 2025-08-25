// file: src/lexer.cpp
#include "lexer.h"
#include <cstring>

static bool is_alpha(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

static bool is_digit(char c) {
    return c >= '0' && c <= '9';
}

static bool is_at_end(Lexer* lexer) {
    return lexer->source[lexer->current] == '\0';
}

static char peek(Lexer* lexer) {
    if (is_at_end(lexer)) return '\0';
    return lexer->source[lexer->current];
}

static char peek_next(Lexer* lexer) {
    if (lexer->current + 1 >= strlen(lexer->source)) return '\0';
    return lexer->source[lexer->current + 1];
}

static char advance(Lexer* lexer) {
    lexer->current++;
    return lexer->source[lexer->current - 1];
}

// Consumes a character if it matches the expected one.
static bool match(Lexer* lexer, char expected) {
    if (is_at_end(lexer)) return false;
    if (lexer->source[lexer->current] != expected) return false;
    lexer->current++;
    return true;
}

static Token make_token(Lexer* lexer, TokenType type) {
    return {
        .type = type,
        .lexeme = &lexer->source[lexer->start],
        .length = (int)(lexer->current - lexer->start)
    };
}

static Token error_token(Lexer* lexer, const char* message) {
    fprintf(stderr, "Lexer Error: %s at position %zu\n", message, lexer->start);
    return {.type = TOKEN_ERROR, .lexeme = message, .length = (int)strlen(message)};
}

static void skip_whitespace(Lexer* lexer) {
    for (;;) {
        char c = peek(lexer);
        switch (c) {
            case ' ':
            case '\t':
            case '\r':
            case '\n':
                advance(lexer);
                break;
            case '/':
                if (peek_next(lexer) == '/') {
                    while (peek(lexer) != '\n' && !is_at_end(lexer)) {
                        advance(lexer);
                    }
                } else {
                    return;
                }
                break;
            default:
                return;
        }
    }
}

static Token string(Lexer* lexer) {
    while (peek(lexer) != '"' && !is_at_end(lexer)) {
        advance(lexer);
    }
    if (is_at_end(lexer)) {
        return error_token(lexer, "Unterminated string.");
    }
    advance(lexer);
    return make_token(lexer, TOKEN_STRING_LITERAL);
}

static Token number(Lexer* lexer) {
    while (is_digit(peek(lexer))) {
        advance(lexer);
    }
    if (peek(lexer) == '.' && is_digit(peek_next(lexer))) {
        advance(lexer);
        while (is_digit(peek(lexer))) {
            advance(lexer);
        }
        return make_token(lexer, TOKEN_FLOAT_LITERAL);
    }
    return make_token(lexer, TOKEN_INT_LITERAL);
}

static TokenType check_keyword(const char* start, int length) {
    switch (start[0]) {
        case 'a': 
            if (length == 5) return strncmp(start, "await", 5) == 0 ? TOKEN_KEYWORD_AWAIT : TOKEN_IDENTIFIER; 
            break;
        case 'c': 
            if (length == 5) return strncmp(start, "const", 5) == 0 ? TOKEN_KEYWORD_CONST : TOKEN_IDENTIFIER; 
            break;
        case 'e':
            if (length == 4) return strncmp(start, "else", 4) == 0 ? TOKEN_KEYWORD_ELSE : TOKEN_IDENTIFIER;
            if (length == 6) return strncmp(start, "extend", 6) == 0 ? TOKEN_KEYWORD_EXTEND : TOKEN_IDENTIFIER;
            break;
        case 'f':
            if (length == 2) return strncmp(start, "fn", 2) == 0 ? TOKEN_KEYWORD_FN : TOKEN_IDENTIFIER;
            if (length == 3) return strncmp(start, "for", 3) == 0 ? TOKEN_KEYWORD_FOR : TOKEN_IDENTIFIER;
            break;
        case 'i':
            if (length == 2) return strncmp(start, "if", 2) == 0 ? TOKEN_KEYWORD_IF : TOKEN_IDENTIFIER;
            if (length == 2) return strncmp(start, "in", 2) == 0 ? TOKEN_KEYWORD_IN : TOKEN_IDENTIFIER;
            if (length == 4) return strncmp(start, "impl", 4) == 0 ? TOKEN_KEYWORD_IMPL : TOKEN_IDENTIFIER;
            break;
        case 'l': 
            if (length == 3) return strncmp(start, "let", 3) == 0 ? TOKEN_KEYWORD_LET : TOKEN_IDENTIFIER; 
            break;
        case 'm':
            if (length == 3) return strncmp(start, "mut", 3) == 0 ? TOKEN_KEYWORD_MUT : TOKEN_IDENTIFIER;
            if (length == 5) return strncmp(start, "match", 5) == 0 ? TOKEN_KEYWORD_MATCH : TOKEN_IDENTIFIER;
            if (length == 6) return strncmp(start, "module", 6) == 0 ? TOKEN_KEYWORD_MODULE : TOKEN_IDENTIFIER;
            break;
        case 'p':
            if (length == 3) return strncmp(start, "pub", 3) == 0 ? TOKEN_KEYWORD_PUB : TOKEN_IDENTIFIER;
            if (length == 8) return strncmp(start, "protocol", 8) == 0 ? TOKEN_KEYWORD_PROTOCOL : TOKEN_IDENTIFIER;
            break;
        case 'r': 
            if (length == 6) return strncmp(start, "return", 6) == 0 ? TOKEN_KEYWORD_RETURN : TOKEN_IDENTIFIER; 
            if (length == 6) return strncmp(start, "record", 6) == 0 ? TOKEN_KEYWORD_RECORD : TOKEN_IDENTIFIER; 
            break;
        case 's': 
            if (length == 5) return strncmp(start, "scope", 5) == 0 ? TOKEN_KEYWORD_SCOPE : TOKEN_IDENTIFIER; 
            if (length == 5) return strncmp(start, "spawn", 5) == 0 ? TOKEN_KEYWORD_SPAWN : TOKEN_IDENTIFIER; 
            break;
        case 't':
            if (length == 3) return strncmp(start, "try", 3) == 0 ? TOKEN_KEYWORD_TRY : TOKEN_IDENTIFIER;
            if (length == 4) return strncmp(start, "type", 4) == 0 ? TOKEN_KEYWORD_TYPE : TOKEN_IDENTIFIER;
            break;
        case 'u':
            if (length == 3) return strncmp(start, "use", 3) == 0 ? TOKEN_KEYWORD_USE : TOKEN_IDENTIFIER;
            if (length == 6) return strncmp(start, "unsafe", 6) == 0 ? TOKEN_KEYWORD_UNSAFE : TOKEN_IDENTIFIER;
            if (length == 5) return strncmp(start, "using", 5) == 0 ? TOKEN_KEYWORD_USING : TOKEN_IDENTIFIER;
            break;
        case 'w': 
            if (length == 5) return strncmp(start, "while", 5) == 0 ? TOKEN_KEYWORD_WHILE : TOKEN_IDENTIFIER; 
            break;
        case 'y': 
            if (length == 5) return strncmp(start, "yield", 5) == 0 ? TOKEN_KEYWORD_YIELD : TOKEN_IDENTIFIER; 
            break;
    }
    return TOKEN_IDENTIFIER;
}

static Token identifier(Lexer* lexer) {
    while (is_alpha(peek(lexer)) || is_digit(peek(lexer))) {
        advance(lexer);
    }
    int length = (int)(lexer->current - lexer->start);
    TokenType type = check_keyword(&lexer->source[lexer->start], length);
    return make_token(lexer, type);
}


void quastra_lexer_init(Lexer* lexer, const char* source) {
    lexer->source = source;
    lexer->start = 0;
    lexer->current = 0;
}

Token quastra_lexer_scan_token(Lexer* lexer) {
    skip_whitespace(lexer);
    lexer->start = lexer->current;
    
    if (is_at_end(lexer)) {
        return make_token(lexer, TOKEN_EOF);
    }
    
    char c = advance(lexer);

    if (is_alpha(c)) return identifier(lexer);
    if (is_digit(c)) return number(lexer);

    switch (c) {
        case '(': return make_token(lexer, TOKEN_LEFT_PAREN);
        case ')': return make_token(lexer, TOKEN_RIGHT_PAREN);
        case '{': return make_token(lexer, TOKEN_LEFT_BRACE);
        case '}': return make_token(lexer, TOKEN_RIGHT_BRACE);
        case '[': return make_token(lexer, TOKEN_LEFT_BRACKET);
        case ']': return make_token(lexer, TOKEN_RIGHT_BRACKET);
        case ';': return make_token(lexer, TOKEN_SEMICOLON);
        case ',': return make_token(lexer, TOKEN_COMMA);
        case '.': return make_token(lexer, TOKEN_DOT);
        case ':': return make_token(lexer, TOKEN_COLON);
        case '"': return string(lexer);
        case '=': return make_token(lexer, match(lexer, '=') ? TOKEN_EQUAL : TOKEN_ASSIGN);
        case '!': return make_token(lexer, match(lexer, '=') ? TOKEN_BANG_EQUAL : TOKEN_BANG);
        case '+': return make_token(lexer, match(lexer, '=') ? TOKEN_PLUS_ASSIGN : TOKEN_PLUS);
        case '-': return make_token(lexer, match(lexer, '>') ? TOKEN_MINUS_GREATER : TOKEN_MINUS);
        case '*': return make_token(lexer, match(lexer, '=') ? TOKEN_STAR_ASSIGN : TOKEN_STAR);
        case '/': return make_token(lexer, match(lexer, '=') ? TOKEN_SLASH_ASSIGN : TOKEN_SLASH);
        case '<': return make_token(lexer, match(lexer, '=') ? TOKEN_LESS_EQUAL : TOKEN_LESS);
        case '>': return make_token(lexer, match(lexer, '=') ? TOKEN_GREATER_EQUAL : TOKEN_GREATER);
        case '|': return make_token(lexer, match(lexer, '|') ? TOKEN_PIPE_PIPE : TOKEN_PIPE);
        case '&': return make_token(lexer, match(lexer, '&') ? TOKEN_AMPERSAND_AMPERSAND : TOKEN_AMPERSAND);
        case '^': return make_token(lexer, TOKEN_CARET);
        default:
            return error_token(lexer, "Unexpected character.");
    }
    return make_token(lexer, TOKEN_ERROR);
}
