#pragma once

#include "../frontend/ast.hpp"
#include <string>
#include <vector>
#include <memory>
#include <sstream>

namespace Quastra {

// The CodeGen class walks the AST and generates equivalent C++ source code.
class CodeGen : public AST::ExprVisitor, public AST::StmtVisitor {
public:
    // The main entry point. Takes an AST and returns a string of C++ code.
    std::string generate(const std::vector<std::unique_ptr<AST::Stmt>>& statements);

private:
    // Statement visitors
    void visit(const AST::ExprStmt& stmt) override;
    void visit(const AST::VarDecl& stmt) override;
    void visit(const AST::Block& stmt) override;
    void visit(const AST::IfStmt& stmt) override;
    void visit(const AST::WhileStmt& stmt) override;
    void visit(const AST::FunctionStmt& stmt) override;
    void visit(const AST::ReturnStmt& stmt) override;

    // Expression visitors
    void visit(const AST::Literal& expr) override;
    void visit(const AST::Unary& expr) override;
    void visit(const AST::Binary& expr) override;
    void visit(const AST::Variable& expr) override;
    void visit(const AST::Assign& expr) override;
    void visit(const AST::Call& expr) override;

    // Helper to generate code for a single node.
    void generate_code(const AST::Stmt& stmt);
    void generate_code(const AST::Expr& expr);

    std::stringstream output;
    int indent_level = 0;

    void indent();
};

} // namespace Quastra
