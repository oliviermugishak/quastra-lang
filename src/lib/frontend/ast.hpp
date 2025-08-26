#pragma once

#include "token.hpp"
#include <memory>
#include <vector>

namespace Quastra::AST {

// Forward declare all node types
struct Literal;
struct Unary;
struct Binary;
struct Variable;
struct Assign;
struct Call;
struct VarDecl;
struct ExprStmt;
struct Block;
struct IfStmt;
struct WhileStmt;
struct FunctionStmt;
struct ReturnStmt;

// Visitor for Statements
struct StmtVisitor {
    virtual ~StmtVisitor() = default;
    virtual void visit(const VarDecl& stmt) = 0;
    virtual void visit(const ExprStmt& stmt) = 0;
    virtual void visit(const Block& stmt) = 0;
    virtual void visit(const IfStmt& stmt) = 0;
    virtual void visit(const WhileStmt& stmt) = 0;
    virtual void visit(const FunctionStmt& stmt) = 0;
    virtual void visit(const ReturnStmt& stmt) = 0;
};

// Base class for all statement nodes
struct Stmt {
    virtual ~Stmt() = default;
    virtual void accept(StmtVisitor& visitor) const = 0;
};

// Visitor for Expressions
struct ExprVisitor {
    virtual ~ExprVisitor() = default;
    virtual void visit(const Literal& expr) = 0;
    virtual void visit(const Unary& expr) = 0;
    virtual void visit(const Binary& expr) = 0;
    virtual void visit(const Variable& expr) = 0;
    virtual void visit(const Assign& expr) = 0;
    virtual void visit(const Call& expr) = 0;
};

// Base class for all expression nodes
struct Expr {
    virtual ~Expr() = default;
    virtual void accept(ExprVisitor& visitor) const = 0;
};

// --- Statement Nodes ---

struct VarDecl : Stmt {
    Token name;
    std::unique_ptr<Expr> initializer;
    VarDecl(Token name, std::unique_ptr<Expr> initializer)
        : name(std::move(name)), initializer(std::move(initializer)) {}
    void accept(StmtVisitor& visitor) const override { visitor.visit(*this); }
};

struct ExprStmt : Stmt {
    std::unique_ptr<Expr> expression;
    ExprStmt(std::unique_ptr<Expr> expression)
        : expression(std::move(expression)) {}
    void accept(StmtVisitor& visitor) const override { visitor.visit(*this); }
};

struct Block : Stmt {
    std::vector<std::unique_ptr<Stmt>> statements;
    Block(std::vector<std::unique_ptr<Stmt>> statements)
        : statements(std::move(statements)) {}
    void accept(StmtVisitor& visitor) const override { visitor.visit(*this); }
};

struct IfStmt : Stmt {
    std::unique_ptr<Expr> condition;
    std::unique_ptr<Stmt> then_branch;
    std::unique_ptr<Stmt> else_branch;
    IfStmt(std::unique_ptr<Expr> c, std::unique_ptr<Stmt> t, std::unique_ptr<Stmt> e)
        : condition(std::move(c)), then_branch(std::move(t)), else_branch(std::move(e)) {}
    void accept(StmtVisitor& visitor) const override { visitor.visit(*this); }
};

struct WhileStmt : Stmt {
    std::unique_ptr<Expr> condition;
    std::unique_ptr<Stmt> body;
    WhileStmt(std::unique_ptr<Expr> c, std::unique_ptr<Stmt> b)
        : condition(std::move(c)), body(std::move(b)) {}
    void accept(StmtVisitor& visitor) const override { visitor.visit(*this); }
};

// Represents a function declaration.
struct FunctionStmt : Stmt {
    Token name;
    std::vector<Token> params;
    std::vector<std::unique_ptr<Stmt>> body;
    FunctionStmt(Token name, std::vector<Token> params, std::vector<std::unique_ptr<Stmt>> body)
        : name(std::move(name)), params(std::move(params)), body(std::move(body)) {}
    void accept(StmtVisitor& visitor) const override { visitor.visit(*this); }
};

// Represents a return statement.
struct ReturnStmt : Stmt {
    Token keyword;
    std::unique_ptr<Expr> value;
    ReturnStmt(Token keyword, std::unique_ptr<Expr> value)
        : keyword(std::move(keyword)), value(std::move(value)) {}
    void accept(StmtVisitor& visitor) const override { visitor.visit(*this); }
};


// --- Expression Nodes ---

struct Literal : Expr {
    Token value;
    Literal(Token val) : value(std::move(val)) {}
    void accept(ExprVisitor& visitor) const override { visitor.visit(*this); }
};

struct Unary : Expr {
    Token op;
    std::unique_ptr<Expr> right;
    Unary(Token o, std::unique_ptr<Expr> r) : op(std::move(o)), right(std::move(r)) {}
    void accept(ExprVisitor& visitor) const override { visitor.visit(*this); }
};

struct Binary : Expr {
    std::unique_ptr<Expr> left;
    Token op;
    std::unique_ptr<Expr> right;
    Binary(std::unique_ptr<Expr> l, Token o, std::unique_ptr<Expr> r)
        : left(std::move(l)), op(std::move(o)), right(std::move(r)) {}
    void accept(ExprVisitor& visitor) const override { visitor.visit(*this); }
};

struct Variable : Expr {
    Token name;
    Variable(Token n) : name(std::move(n)) {}
    void accept(ExprVisitor& visitor) const override { visitor.visit(*this); }
};

struct Assign : Expr {
    Token name;
    std::unique_ptr<Expr> value;
    Assign(Token n, std::unique_ptr<Expr> v) : name(std::move(n)), value(std::move(v)) {}
    void accept(ExprVisitor& visitor) const override { visitor.visit(*this); }
};

// Represents a function call.
struct Call : Expr {
    std::unique_ptr<Expr> callee;
    Token paren; // The closing ')' for error reporting.
    std::vector<std::unique_ptr<Expr>> arguments;
    Call(std::unique_ptr<Expr> callee, Token paren, std::vector<std::unique_ptr<Expr>> arguments)
        : callee(std::move(callee)), paren(std::move(paren)), arguments(std::move(arguments)) {}
    void accept(ExprVisitor& visitor) const override { visitor.visit(*this); }
};

} // namespace Quastra::AST
