#include "resolver.hpp"
#include <iostream>

namespace Quastra {

bool Resolver::resolve(const std::vector<std::unique_ptr<AST::Stmt>>& statements) {
    // Create the global scope before starting.
    begin_scope();
    for (const auto& statement : statements) {
        if (statement) {
            statement->accept(*this);
        }
    }
    // Clean up the global scope at the end.
    end_scope();
    return !had_error;
}

void Resolver::begin_scope() {
    scopes.emplace_back();
}

void Resolver::end_scope() {
    scopes.pop_back();
}

// --- Visitor Implementations ---

void Resolver::visit(const AST::Block& stmt) {
    begin_scope();
    for (const auto& statement : stmt.statements) {
        statement->accept(*this);
    }
    end_scope();
}

void Resolver::visit(const AST::VarDecl& stmt) {
    // Check if we are redeclaring a variable in the current scope.
    if (!scopes.empty()) {
        auto& current_scope = scopes.back();
        if (current_scope.count(stmt.name.lexeme)) {
            std::cerr << "Semantic Error: Variable '" << stmt.name.lexeme << "' already declared in this scope.\n";
            had_error = true;
        }
    }

    // Add the variable to the current scope.
    // We mark it as "defined" but not yet "initialized".
    if (!scopes.empty()) {
        scopes.back()[stmt.name.lexeme] = false;
    }

    // If there is an initializer, resolve it.
    if (stmt.initializer) {
        stmt.initializer->accept(*this);
    }

    // Now the variable is initialized and ready for use.
    if (!scopes.empty()) {
        scopes.back()[stmt.name.lexeme] = true;
    }
}

void Resolver::visit(const AST::Variable& expr) {
    // Check if the variable exists in any scope, starting from the innermost.
    for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
        if (it->count(expr.name.lexeme)) {
            // Found it. A real compiler might store more info here.
            return;
        }
    }

    std::cerr << "Semantic Error: Undefined variable '" << expr.name.lexeme << "'.\n";
    had_error = true;
}

void Resolver::visit(const AST::Assign& expr) {
    // First, resolve the expression being assigned to ensure it's valid.
    expr.value->accept(*this);
    // Then, check if the variable we're assigning to exists.
    for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
        if (it->count(expr.name.lexeme)) {
            return; // It's a valid assignment target.
        }
    }

    std::cerr << "Semantic Error: Assignment to undeclared variable '" << expr.name.lexeme << "'.\n";
    had_error = true;
}


// --- Stubs for other nodes ---
// We need to make sure we visit all children of these nodes.

void Resolver::visit(const AST::ExprStmt& stmt) {
    stmt.expression->accept(*this);
}

void Resolver::visit(const AST::IfStmt& stmt) {
    stmt.condition->accept(*this);
    stmt.then_branch->accept(*this);
    if (stmt.else_branch) {
        stmt.else_branch->accept(*this);
    }
}

void Resolver::visit(const AST::WhileStmt& stmt) {
    stmt.condition->accept(*this);
    stmt.body->accept(*this);
}

void Resolver::visit(const AST::FunctionStmt& stmt) {
    // For now, we'll just resolve the body.
    // A full implementation would handle parameters.
    begin_scope();
    for(const auto& s : stmt.body) {
        s->accept(*this);
    }
    end_scope();
}

void Resolver::visit(const AST::ReturnStmt& stmt) {
    if (stmt.value) {
        stmt.value->accept(*this);
    }
}

void Resolver::visit(const AST::Literal& expr) { (void)expr; /* Literals need no resolution */ }

void Resolver::visit(const AST::Unary& expr) {
    expr.right->accept(*this);
}

void Resolver::visit(const AST::Binary& expr) {
    expr.left->accept(*this);
    expr.right->accept(*this);
}

void Resolver::visit(const AST::Call& expr) {
    expr.callee->accept(*this);
    for (const auto& arg : expr.arguments) {
        arg->accept(*this);
    }
}

} // namespace Quastra
