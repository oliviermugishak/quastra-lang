#pragma once

#include "token.hpp"
#include <memory>
#include <vector>

namespace Quastra::AST {

// Forward declare all node types
struct Literal;
struct Unary;
struct Binary;
struct VarDecl;
struct ExprStmt;

// Visitor for Statements
struct StmtVisitor {
    virtual ~StmtVisitor() = default;
    virtual void visit(const VarDecl& stmt) = 0;
    virtual void visit(const ExprStmt& stmt) = 0;
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
};

// Base class for all expression nodes
struct Expr {
    virtual ~Expr() = default;
    virtual void accept(ExprVisitor& visitor) const = 0;
};

// --- Statement Nodes ---

// Represents a variable declaration: `let x = 5;`
struct VarDecl : Stmt {
    Token name;
    std::unique_ptr<Expr> initializer;

    VarDecl(Token name, std::unique_ptr<Expr> initializer)
        : name(std::move(name)), initializer(std::move(initializer)) {}

    void accept(StmtVisitor& visitor) const override {
        visitor.visit(*this);
    }
};

// Represents an expression that is used as a statement.
struct ExprStmt : Stmt {
    std::unique_ptr<Expr> expression;

    ExprStmt(std::unique_ptr<Expr> expression)
        : expression(std::move(expression)) {}

    void accept(StmtVisitor& visitor) const override {
        visitor.visit(*this);
    }
};


// --- Expression Nodes ---

struct Literal : Expr {
    Token value;
    Literal(Token value) : value(std::move(value)) {}
    void accept(ExprVisitor& visitor) const override { visitor.visit(*this); }
};

struct Unary : Expr {
    Token op;
    std::unique_ptr<Expr> right;
    Unary(Token op, std::unique_ptr<Expr> right)
        : op(std::move(op)), right(std::move(right)) {}
    void accept(ExprVisitor& visitor) const override { visitor.visit(*this); }
};

struct Binary : Expr {
    std::unique_ptr<Expr> left;
    Token op;
    std::unique_ptr<Expr> right;
    Binary(std::unique_ptr<Expr> left, Token op, std::unique_ptr<Expr> right)
        : left(std::move(left)), op(std::move(op)), right(std::move(right)) {}
    void accept(ExprVisitor& visitor) const override { visitor.visit(*this); }
};

} // namespace Quastra::AST
