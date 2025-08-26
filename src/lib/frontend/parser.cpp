#include "parser.hpp"
#include <stdexcept>
#include <vector>

namespace Quastra {

Parser::Parser(const std::vector<Token>& tokens) : tokens(tokens) {}

// Main entry point: parse a sequence of statements.
std::vector<std::unique_ptr<AST::Stmt>> Parser::parse() {
    std::vector<std::unique_ptr<AST::Stmt>> statements;
    while (!is_at_end()) {
        statements.push_back(declaration());
    }
    return statements;
}

// declaration -> var_declaration | statement
std::unique_ptr<AST::Stmt> Parser::declaration() {
    try {
        if (match({TokenType::Let})) return var_declaration();
        return statement();
    } catch (const std::runtime_error& e) {
        // Synchronize to the next statement on error.
        // A real compiler would have more advanced error recovery.
        return nullptr;
    }
}

// var_declaration -> "let" IDENTIFIER ( "=" expression )? ";"
std::unique_ptr<AST::Stmt> Parser::var_declaration() {
    Token name = consume(TokenType::Identifier, "Expect variable name.");

    std::unique_ptr<AST::Expr> initializer = nullptr;
    if (match({TokenType::Equal})) {
        initializer = expression();
    }

    consume(TokenType::Semicolon, "Expect ';' after variable declaration.");
    return std::make_unique<AST::VarDecl>(name, std::move(initializer));
}

// statement -> expression_statement
std::unique_ptr<AST::Stmt> Parser::statement() {
    return expression_statement();
}

// expression_statement -> expression ";"
std::unique_ptr<AST::Stmt> Parser::expression_statement() {
    std::unique_ptr<AST::Expr> expr = expression();
    consume(TokenType::Semicolon, "Expect ';' after expression.");
    return std::make_unique<AST::ExprStmt>(std::move(expr));
}

// --- Expression Parsing Logic (largely unchanged) ---

std::unique_ptr<AST::Expr> Parser::expression() {
    return term();
}

std::unique_ptr<AST::Expr> Parser::term() {
    std::unique_ptr<AST::Expr> expr = factor();
    while (match({TokenType::Minus, TokenType::Plus})) {
        Token op = previous();
        expr = std::make_unique<AST::Binary>(std::move(expr), op, factor());
    }
    return expr;
}

std::unique_ptr<AST::Expr> Parser::factor() {
    std::unique_ptr<AST::Expr> expr = unary();
    while (match({TokenType::Slash, TokenType::Star})) {
        Token op = previous();
        expr = std::make_unique<AST::Binary>(std::move(expr), op, unary());
    }
    return expr;
}

std::unique_ptr<AST::Expr> Parser::unary() {
    if (match({TokenType::Bang, TokenType::Minus})) {
        Token op = previous();
        return std::make_unique<AST::Unary>(op, unary());
    }
    return primary();
}

std::unique_ptr<AST::Expr> Parser::primary() {
    if (match({TokenType::IntLiteral})) {
        return std::make_unique<AST::Literal>(previous());
    }
    if (match({TokenType::LeftParen})) {
        std::unique_ptr<AST::Expr> expr = expression();
        consume(TokenType::RightParen, "Expect ')' after expression.");
        return expr;
    }
    throw std::runtime_error("Parser Error: Expected expression.");
}

// --- Helper Methods ---

bool Parser::match(const std::vector<TokenType>& types) {
    for (TokenType type : types) {
        if (!is_at_end() && peek().type == type) {
            advance();
            return true;
        }
    }
    return false;
}

Token Parser::consume(TokenType type, const std::string& message) {
    if (!is_at_end() && peek().type == type) return advance();
    throw std::runtime_error(message);
}

Token Parser::advance() {
    if (!is_at_end()) current++;
    return previous();
}

bool Parser::is_at_end() {
    return peek().type == TokenType::EndOfFile;
}

Token Parser::peek() {
    return tokens[current];
}

Token Parser::previous() {
    return tokens[current - 1];
}

} // namespace Quastra
