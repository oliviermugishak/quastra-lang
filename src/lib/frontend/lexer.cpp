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
    tokens.push_back({TokenType::EndOfFile, "", line});
    return tokens;
}

bool Lexer::is_at_end() {
    return current >= source.length();
}

char Lexer::peek() {
    if (is_at_end()) return '\0';
    return source[current];
}

char Lexer::advance() {
    return source[current++];
}

bool Lexer::match(char expected) {
    if (is_at_end()) return false;
    if (source[current] != expected) return false;
    current++;
    return true;
}

void Lexer::add_token(TokenType type) {
    std::string text = source.substr(start, current - start);
    tokens.push_back({type, text, line});
}

void Lexer::scan_token() {
    char c = advance();
    switch (c) {
        case '(': add_token(TokenType::LeftParen); break;
        case ')': add_token(TokenType::RightParen); break;
        case '{': add_token(TokenType::LeftBrace); break;
        case '}': add_token(TokenType::RightBrace); break;
        case ',': add_token(TokenType::Comma); break;
        case '+': add_token(TokenType::Plus); break;
        case '*': add_token(TokenType::Star); break;
        case ';': add_token(TokenType::Semicolon); break;
        case '!': add_token(match('=') ? TokenType::BangEqual : TokenType::Bang); break;
        case '=': add_token(match('=') ? TokenType::EqualEqual : TokenType::Equal); break;
        case '<': add_token(match('=') ? TokenType::LessEqual : TokenType::Less); break;
        case '>': add_token(match('=') ? TokenType::GreaterEqual : TokenType::Greater); break;
        case '/':
            if (match('/')) {
                while (peek() != '\n' && !is_at_end()) advance();
            } else {
                add_token(TokenType::Slash);
            }
            break;
        case '-':
            if (match('>')) {
                add_token(TokenType::Arrow);
            } else {
                add_token(TokenType::Minus);
            }
            break;
        case ' ':
        case '\r':
        case '\t':
            break;
        case '\n':
            line++;
            break;
        default:
            if (std::isdigit(c)) {
                number();
            } else if (std::isalpha(c) || c == '_') {
                identifier();
            } else {
                add_token(TokenType::Unknown);
            }
            break;
    }
}

void Lexer::number() {
    while (std::isdigit(peek())) advance();
    add_token(TokenType::IntLiteral);
}

void Lexer::identifier() {
    while (std::isalnum(peek()) || peek() == '_') advance();
    std::string text = source.substr(start, current - start);
    static const std::map<std::string, TokenType> keywords = {
        {"fn", TokenType::Fn}, {"return", TokenType::Return},
        {"let", TokenType::Let}, {"mut", TokenType::Mut},
        {"if", TokenType::If}, {"else", TokenType::Else},
        {"while", TokenType::While}, {"true", TokenType::True},
        {"false", TokenType::False}, {"int", TokenType::TypeIdentifier},
        {"string", TokenType::TypeIdentifier}, {"bool", TokenType::TypeIdentifier},
        {"float", TokenType::TypeIdentifier},
    };
    auto it = keywords.find(text);
    if (it != keywords.end()) {
        add_token(it->second);
    } else {
        add_token(TokenType::Identifier);
    }
}

} // namespace Quastra
