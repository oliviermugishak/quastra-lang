#include "lexer.hpp"
#include <map>
#include <cctype> // For isdigit, isalpha, etc.

namespace Quastra {

Lexer::Lexer(const std::string& source) : source(source) {}

std::vector<Token> Lexer::scan_tokens() {
    while (!is_at_end()) {
        start = current;
        scan_token();
    }

    // Add one final EndOfFile token to signify completion.
    tokens.push_back({TokenType::EndOfFile, "", line});
    return tokens;
}

bool Lexer::is_at_end() {
    return current >= source.length();
}

char Lexer::advance() {
    return source[current++];
}

char Lexer::peek() {
    if (is_at_end()) return '\0';
    return source[current];
}

Token Lexer::make_token(TokenType type) {
    return {type, source.substr(start, current - start), line};
}

void Lexer::scan_token() {
    char c = advance();
    switch (c) {
        // Single-character tokens
        case '(': tokens.push_back(make_token(TokenType::LeftParen)); break;
        case ')': tokens.push_back(make_token(TokenType::RightParen)); break;
        case '{': tokens.push_back(make_token(TokenType::LeftBrace)); break;
        case '}': tokens.push_back(make_token(TokenType::RightBrace)); break;
        case '+': tokens.push_back(make_token(TokenType::Plus)); break;
        case '*': tokens.push_back(make_token(TokenType::Star)); break;
        case ';': tokens.push_back(make_token(TokenType::Semicolon)); break;
        case '=': tokens.push_back(make_token(TokenType::Equal)); break;


        // Handle '/' for division or comments
        case '/':
            if (peek() == '/') {
                // A comment goes until the end of the line, so we skip it.
                while (peek() != '\n' && !is_at_end()) advance();
            } else {
                tokens.push_back(make_token(TokenType::Slash));
            }
            break;

        // Handle '-' for minus or arrow
        case '-':
            if (peek() == '>') {
                advance(); // consume '>'
                tokens.push_back(make_token(TokenType::Arrow));
            } else {
                tokens.push_back(make_token(TokenType::Minus));
            }
            break;

        // Ignore whitespace
        case ' ':
        case '\r':
        case '\t':
            break;

        case '\n':
            line++;
            break;

        default:
            if (std::isdigit(c)) {
                tokens.push_back(number());
            } else if (std::isalpha(c) || c == '_') {
                tokens.push_back(identifier());
            } else {
                tokens.push_back(make_token(TokenType::Unknown));
            }
            break;
    }
}

Token Lexer::number() {
    while (std::isdigit(peek())) advance();
    return make_token(TokenType::IntLiteral);
}

Token Lexer::identifier() {
    while (std::isalnum(peek()) || peek() == '_') advance();

    std::string text = source.substr(start, current - start);

    static const std::map<std::string, TokenType> keywords = {
        {"fn", TokenType::Fn},
        {"return", TokenType::Return},
        {"let", TokenType::Let},
        {"mut", TokenType::Mut},
        {"int", TokenType::TypeIdentifier},
        {"string", TokenType::TypeIdentifier},
        {"bool", TokenType::TypeIdentifier},
        {"float", TokenType::TypeIdentifier},
    };

    auto it = keywords.find(text);
    if (it != keywords.end()) {
        return make_token(it->second);
    }

    return make_token(TokenType::Identifier);
}

} // namespace Quastra
