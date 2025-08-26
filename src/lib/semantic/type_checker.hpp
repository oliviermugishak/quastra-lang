#pragma once

#include "../frontend/ast.hpp"
#include "type.hpp"
#include "symbol.hpp" // Include the new Symbol struct
#include <vector>
#include <memory>
#include <map>
#include <string>

namespace Quastra {

class TypeChecker : public AST::ExprVisitor, public AST::StmtVisitor {
public:
    bool check(const std::vector<std::unique_ptr<AST::Stmt>>& statements);

private:
    void begin_scope();
    void end_scope();
    Type type_of(const AST::Expr& expr);
    void check_type(Type expected, Type actual, const std::string& error_message);
    void define(const Token& name, const Symbol& symbol);
    const Symbol* resolve(const Token& name);

    // Statement visitors
    void visit(const AST::Block& stmt) override;
    void visit(const AST::VarDecl& stmt) override;
    void visit(const AST::ExprStmt& stmt) override;
    void visit(const AST::IfStmt& stmt) override;
    void visit(const AST::WhileStmt& stmt) override;
    void visit(const AST::FunctionStmt& stmt) override;
    void visit(const AST::ReturnStmt& stmt) override;

    // Expression visitors
    Type last_type = Type::Void;
    void visit(const AST::Literal& expr) override;
    void visit(const AST::Unary& expr) override;
    void visit(const AST::Binary& expr) override;
    void visit(const AST::Variable& expr) override;
    void visit(const AST::Assign& expr) override;
    // Remove the stub body for Call
    void visit(const AST::Call& expr) override;

    // The Symbol Table now stores Symbol structs.
    std::vector<std::map<std::string, Symbol>> scopes;
    bool had_error = false;
    Type current_function_return_type = Type::Void;
};

} // namespace Quastra
