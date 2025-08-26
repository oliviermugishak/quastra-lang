#pragma once

#include "token.hpp"
#include "ast.hpp"
#include <vector>
#include <memory>

namespace Quastra {

class Parser {
public:
    Parser(const std::vector<Token>& tokens);

    // The main entry point now returns a list of statements.
    std::vector<std::unique_ptr<AST::Stmt>> parse();

private:
    // Statement parsing
    std::unique_ptr<AST::Stmt> declaration();
    std::unique_ptr<AST::Stmt> var_declaration();
    std::unique_ptr<AST::Stmt> statement();
    std::unique_ptr<AST::Stmt> expression_statement();

    // Expression parsing (from before)
    std::unique_ptr<AST::Expr> expression();
    std::unique_ptr<AST::Expr> term();
    std::unique_ptr<AST::Expr> factor();
    std::unique_ptr<AST::Expr> unary();
    std::unique_ptr<AST::Expr> primary();

    // Helpers
    bool match(const std::vector<TokenType>& types);
    Token consume(TokenType type, const std::string& message);
    Token advance();
    bool is_at_end();
    Token peek();
    Token previous();

    const std::vector<Token>& tokens;
    size_t current = 0;
};

} // namespace Quastra
