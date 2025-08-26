#pragma once

#include <string>
#include <variant>
#include <ostream> // For printing tokens in test failures

namespace Quastra {

// Enum for all possible token types in the Quastra language.
enum class TokenType {
    // Keywords
    Fn, Return, Let, Mut, If, Else, While, For, In,
    // Identifiers
    Identifier, TypeIdentifier,
    // Literals
    IntLiteral, StringLiteral,
    // Operators
    Plus, Minus, Star, Slash,
    Equal, EqualEqual, Bang, BangEqual,
    Less, LessEqual, Greater, GreaterEqual,
    Arrow, // ->
    // Separators
    LeftParen, RightParen, // ( )
    LeftBrace, RightBrace, // { }
    Semicolon,
    // Meta
    EndOfFile,
    Unknown,
};

// Converts a TokenType to its string representation for debugging.
const char* to_string(TokenType type);

// Represents a single token scanned from the source code.
struct Token {
    TokenType type;
    std::string lexeme;
    int line;

    // Operator for easy comparison in tests.
    bool operator==(const Token& other) const {
        return type == other.type && lexeme == other.lexeme && line == other.line;
    }
};

// Custom stream operator to make Google Test print tokens beautifully on failure.
inline std::ostream& operator<<(std::ostream& os, const Token& token) {
    os << "Token(type: " << to_string(token.type)
       << ", lexeme: '" << token.lexeme
       << "', line: " << token.line << ")";
    return os;
}

} // namespace Quastra
