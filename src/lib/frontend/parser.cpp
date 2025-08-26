#include "parser.hpp"
#include <stdexcept>
#include <vector>

namespace Quastra {

Parser::Parser(const std::vector<Token>& tokens) : tokens(tokens) {}

std::vector<std::unique_ptr<AST::Stmt>> Parser::parse() {
    std::vector<std::unique_ptr<AST::Stmt>> statements;
    while (!is_at_end()) {
        statements.push_back(declaration());
    }
    return statements;
}

std::unique_ptr<AST::Stmt> Parser::declaration() {
    try {
        if (match({TokenType::Fn})) return function_declaration();
        if (match({TokenType::Let})) return var_declaration();
        return statement();
    } catch (const std::runtime_error& e) {
        // A real compiler would synchronize here to recover.
        return nullptr;
    }
}

std::unique_ptr<AST::Stmt> Parser::function_declaration() {
    Token name = consume(TokenType::Identifier, "Expect function name.");
    consume(TokenType::LeftParen, "Expect '(' after function name.");
    std::vector<Token> parameters;
    if (peek().type != TokenType::RightParen) {
        do {
            parameters.push_back(consume(TokenType::Identifier, "Expect parameter name."));
        } while (match({TokenType::Semicolon})); // Placeholder for comma
    }
    consume(TokenType::RightParen, "Expect ')' after parameters.");
    consume(TokenType::LeftBrace, "Expect '{' before function body.");
    std::vector<std::unique_ptr<AST::Stmt>> body = block();
    return std::make_unique<AST::FunctionStmt>(name, std::move(parameters), std::move(body));
}

std::unique_ptr<AST::Stmt> Parser::var_declaration() {
    Token name = consume(TokenType::Identifier, "Expect variable name.");
    std::unique_ptr<AST::Expr> initializer = nullptr;
    if (match({TokenType::Equal})) {
        initializer = expression();
    }
    consume(TokenType::Semicolon, "Expect ';' after variable declaration.");
    return std::make_unique<AST::VarDecl>(name, std::move(initializer));
}

std::unique_ptr<AST::Stmt> Parser::statement() {
    if (match({TokenType::If})) return if_statement();
    if (match({TokenType::While})) return while_statement();
    if (match({TokenType::Return})) return return_statement();
    if (match({TokenType::LeftBrace})) return std::make_unique<AST::Block>(block());
    return expression_statement();
}

std::unique_ptr<AST::Stmt> Parser::if_statement() {
    consume(TokenType::LeftParen, "Expect '(' after 'if'.");
    std::unique_ptr<AST::Expr> condition = expression();
    consume(TokenType::RightParen, "Expect ')' after if condition.");
    std::unique_ptr<AST::Stmt> then_branch = statement();
    std::unique_ptr<AST::Stmt> else_branch = nullptr;
    if (match({TokenType::Else})) {
        else_branch = statement();
    }
    return std::make_unique<AST::IfStmt>(std::move(condition), std::move(then_branch), std::move(else_branch));
}

std::unique_ptr<AST::Stmt> Parser::while_statement() {
    consume(TokenType::LeftParen, "Expect '(' after 'while'.");
    std::unique_ptr<AST::Expr> condition = expression();
    consume(TokenType::RightParen, "Expect ')' after while condition.");
    std::unique_ptr<AST::Stmt> body = statement();
    return std::make_unique<AST::WhileStmt>(std::move(condition), std::move(body));
}

std::unique_ptr<AST::Stmt> Parser::return_statement() {
    Token keyword = previous();
    std::unique_ptr<AST::Expr> value = nullptr;
    if (peek().type != TokenType::Semicolon) {
        value = expression();
    }
    consume(TokenType::Semicolon, "Expect ';' after return value.");
    return std::make_unique<AST::ReturnStmt>(keyword, std::move(value));
}

std::vector<std::unique_ptr<AST::Stmt>> Parser::block() {
    std::vector<std::unique_ptr<AST::Stmt>> statements;
    while (peek().type != TokenType::RightBrace && !is_at_end()) {
        statements.push_back(declaration());
    }
    consume(TokenType::RightBrace, "Expect '}' after block.");
    return statements;
}

std::unique_ptr<AST::Stmt> Parser::expression_statement() {
    std::unique_ptr<AST::Expr> expr = expression();
    consume(TokenType::Semicolon, "Expect ';' after expression.");
    return std::make_unique<AST::ExprStmt>(std::move(expr));
}

std::unique_ptr<AST::Expr> Parser::expression() {
    return assignment();
}

std::unique_ptr<AST::Expr> Parser::assignment() {
    std::unique_ptr<AST::Expr> expr = equality();
    if (match({TokenType::Equal})) {
        Token equals = previous();
        std::unique_ptr<AST::Expr> value = assignment();
        if (auto* var = dynamic_cast<AST::Variable*>(expr.get())) {
            return std::make_unique<AST::Assign>(var->name, std::move(value));
        }
        throw std::runtime_error("Invalid assignment target.");
    }
    return expr;
}

std::unique_ptr<AST::Expr> Parser::equality() {
    std::unique_ptr<AST::Expr> expr = comparison();
    while (match({TokenType::BangEqual, TokenType::EqualEqual})) {
        Token op = previous();
        expr = std::make_unique<AST::Binary>(std::move(expr), op, comparison());
    }
    return expr;
}

std::unique_ptr<AST::Expr> Parser::comparison() {
    std::unique_ptr<AST::Expr> expr = term();
    while (match({TokenType::Greater, TokenType::GreaterEqual, TokenType::Less, TokenType::LessEqual})) {
        Token op = previous();
        expr = std::make_unique<AST::Binary>(std::move(expr), op, term());
    }
    return expr;
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
    return call();
}

std::unique_ptr<AST::Expr> Parser::call() {
    std::unique_ptr<AST::Expr> expr = primary();
    while (true) {
        if (match({TokenType::LeftParen})) {
            std::vector<std::unique_ptr<AST::Expr>> arguments;
            if (peek().type != TokenType::RightParen) {
                do {
                    arguments.push_back(expression());
                } while (match({TokenType::Semicolon})); // Placeholder
            }
            Token paren = consume(TokenType::RightParen, "Expect ')' after arguments.");
            expr = std::make_unique<AST::Call>(std::move(expr), paren, std::move(arguments));
        } else {
            break;
        }
    }
    return expr;
}

std::unique_ptr<AST::Expr> Parser::primary() {
    if (match({TokenType::False})) return std::make_unique<AST::Literal>(Token{TokenType::False, "false", previous().line});
    if (match({TokenType::True})) return std::make_unique<AST::Literal>(Token{TokenType::True, "true", previous().line});
    if (match({TokenType::IntLiteral})) return std::make_unique<AST::Literal>(previous());
    if (match({TokenType::Identifier})) return std::make_unique<AST::Variable>(previous());
    if (match({TokenType::LeftParen})) {
        std::unique_ptr<AST::Expr> expr = expression();
        consume(TokenType::RightParen, "Expect ')' after expression.");
        return expr;
    }
    throw std::runtime_error("Parser Error: Expected expression.");
}

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
