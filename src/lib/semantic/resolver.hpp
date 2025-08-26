#pragma once

#include "../frontend/ast.hpp"
#include "type.hpp"
#include <vector>
#include <memory>
#include <map>
#include <string>

namespace Quastra {

// The Resolver walks the AST to perform semantic analysis, such as
// resolving variables and checking for scope-related errors.
class Resolver : public AST::ExprVisitor, public AST::StmtVisitor {
public:
    // The main entry point. Takes an AST and returns true if no errors were found.
    bool resolve(const std::vector<std::unique_ptr<AST::Stmt>>& statements);

private:
    // Scope management
    void begin_scope();
    void end_scope();

    // Statement visitors
    void visit(const AST::Block& stmt) override;
    void visit(const AST::VarDecl& stmt) override;
    void visit(const AST::Variable& expr) override;
    void visit(const AST::Assign& expr) override;

    // --- Stubs for other nodes (we'll implement these as we go) ---
    void visit(const AST::ExprStmt& stmt) override;
    void visit(const AST::IfStmt& stmt) override;
    void visit(const AST::WhileStmt& stmt) override;
    void visit(const AST::FunctionStmt& stmt) override;
    void visit(const AST::ReturnStmt& stmt) override;
    void visit(const AST::Literal& expr) override;
    void visit(const AST::Unary& expr) override;
    void visit(const AST::Binary& expr) override;
    void visit(const AST::Call& expr) override;

    // The Symbol Table: a stack of scopes.
    // The map stores variable names and a boolean indicating if they've been initialized.
    std::vector<std::map<std::string, bool>> scopes;
    bool had_error = false;
};

} // namespace Quastra
