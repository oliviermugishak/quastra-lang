#include "token.hpp"

namespace Quastra {

// Implementation of the to_string function for TokenType.
const char* to_string(TokenType type) {
    switch (type) {
        case TokenType::Fn: return "Fn";
        case TokenType::Return: return "Return";
        case TokenType::Let: return "Let";
        case TokenType::If: return "If";
        case TokenType::Else: return "Else";
        case TokenType::While: return "While";
        case TokenType::True: return "True";
        case TokenType::False: return "False";
        case TokenType::Identifier: return "Identifier";
        case TokenType::TypeIdentifier: return "TypeIdentifier";
        case TokenType::IntLiteral: return "IntLiteral";
        case TokenType::Plus: return "Plus";
        case TokenType::Minus: return "Minus";
        case TokenType::Star: return "Star";
        case TokenType::Slash: return "Slash";
        case TokenType::Equal: return "Equal";
        case TokenType::EqualEqual: return "EqualEqual";
        case TokenType::BangEqual: return "BangEqual";
        case TokenType::Less: return "Less";
        case TokenType::LessEqual: return "LessEqual";
        case TokenType::Greater: return "Greater";
        case TokenType::GreaterEqual: return "GreaterEqual";
        case TokenType::Semicolon: return "Semicolon";
        case TokenType::Arrow: return "Arrow";
        case TokenType::LeftParen: return "LeftParen";
        case TokenType::RightParen: return "RightParen";
        case TokenType::LeftBrace: return "LeftBrace";
        case TokenType::RightBrace: return "RightBrace";
        case TokenType::EndOfFile: return "EndOfFile";
        case TokenType::Unknown: return "Unknown";
        default: return "Unhandled TokenType";
    }
}

} // namespace Quastra
