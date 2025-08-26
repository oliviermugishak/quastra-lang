#pragma once

#include "../frontend/ast.hpp"
#include "../runtime/environment.hpp"
#include <vector>
#include <memory>

namespace Quastra {

// The Interpreter walks the AST and executes the code.
class Interpreter : public AST::ExprVisitor, public AST::StmtVisitor {
public:
    Interpreter();

    void interpret(const std::vector<std::unique_ptr<AST::Stmt>>& statements);

    std::shared_ptr<Environment> get_environment() const { return environment; }

protected:
    // Protected so that test subclasses can access it.
    QuastraValue last_evaluated_value;

private:
    // Statement visitors
    void visit(const AST::VarDecl& stmt) override;
    void visit(const AST::ExprStmt& stmt) override;
    // Remove inline definitions for methods now implemented in the .cpp file
    void visit(const AST::Block& stmt) override;
    void visit(const AST::IfStmt& stmt) override;
    void visit(const AST::WhileStmt& stmt) override;
    // Stubs for features not yet implemented
    void visit(const AST::FunctionStmt& stmt) override { (void)stmt; }
    void visit(const AST::ReturnStmt& stmt) override { (void)stmt; }


    // Expression visitors
    void visit(const AST::Literal& expr) override;
    void visit(const AST::Unary& expr) override;
    void visit(const AST::Binary& expr) override;
    void visit(const AST::Variable& expr) override;
    void visit(const AST::Assign& expr) override;
    void visit(const AST::Call& expr) override { (void)expr; }

    // Helper to evaluate an expression node.
    QuastraValue evaluate(const AST::Expr& expr);

    std::shared_ptr<Environment> environment;
};

} // namespace Quastra
