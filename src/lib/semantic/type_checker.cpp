#include "type_checker.hpp"
#include <iostream>

namespace Quastra {

bool TypeChecker::check(const std::vector<std::unique_ptr<AST::Stmt>>& statements) {
    begin_scope();
    for (const auto& statement : statements) {
        if (statement) {
            statement->accept(*this);
        }
    }
    end_scope();
    return !had_error;
}

void TypeChecker::begin_scope() {
    scopes.emplace_back();
}

void TypeChecker::end_scope() {
    scopes.pop_back();
}

void TypeChecker::define(const Token& name, const Symbol& symbol) {
    if (!scopes.empty()) {
        if (scopes.back().count(name.lexeme)) {
            std::cerr << "Semantic Error: Variable '" << name.lexeme << "' already declared in this scope.\n";
            had_error = true;
        }
        scopes.back()[name.lexeme] = symbol;
    }
}

const Symbol* TypeChecker::resolve(const Token& name) {
    for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
        if (it->count(name.lexeme)) {
            return &(*it)[name.lexeme];
        }
    }
    return nullptr;
}


Type TypeChecker::type_of(const AST::Expr& expr) {
    expr.accept(*this);
    return last_type;
}

void TypeChecker::check_type(Type expected, Type actual, const std::string& error_message) {
    if (actual != expected && actual != Type::Error) {
        std::cerr << "Type Error: " << error_message << "\n";
        had_error = true;
    }
}

// --- Statement Visitors ---

void TypeChecker::visit(const AST::Block& stmt) {
    begin_scope();
    for (const auto& statement : stmt.statements) {
        statement->accept(*this);
    }
    end_scope();
}

void TypeChecker::visit(const AST::VarDecl& stmt) {
    Type initializer_type = Type::Void;
    if (stmt.initializer) {
        initializer_type = type_of(*stmt.initializer);
    }
    define(stmt.name, {initializer_type, stmt.is_mutable, true});
}

void TypeChecker::visit(const AST::ExprStmt& stmt) {
    type_of(*stmt.expression);
}

void TypeChecker::visit(const AST::IfStmt& stmt) {
    Type condition_type = type_of(*stmt.condition);
    check_type(Type::Bool, condition_type, "If condition must be a boolean.");
    stmt.then_branch->accept(*this);
    if (stmt.else_branch) {
        stmt.else_branch->accept(*this);
    }
}

void TypeChecker::visit(const AST::WhileStmt& stmt) {
    Type condition_type = type_of(*stmt.condition);
    check_type(Type::Bool, condition_type, "While condition must be a boolean.");
    stmt.body->accept(*this);
}

void TypeChecker::visit(const AST::FunctionStmt& stmt) {
    // For now, we'll assume functions return Int. A full implementation
    // would parse the return type from the function signature.
    Type return_type = Type::Int;
    define(stmt.name, {return_type, false, true});

    Type old_return_type = current_function_return_type;
    current_function_return_type = return_type;

    begin_scope();
    for (const auto& param : stmt.params) {
        // Parameters are immutable by default.
        define(param, {Type::Int, false, true}); // Assume Int for now
    }
    for (const auto& body_stmt : stmt.body) {
        body_stmt->accept(*this);
    }
    end_scope();

    current_function_return_type = old_return_type;
}

void TypeChecker::visit(const AST::ReturnStmt& stmt) {
    if (current_function_return_type == Type::Void) {
        std::cerr << "Semantic Error: Cannot return from top-level code.\n";
        had_error = true;
    }
    if (stmt.value) {
        Type return_value_type = type_of(*stmt.value);
        check_type(current_function_return_type, return_value_type, "Return value type does not match function's return type.");
    }
}


// --- Expression Visitors ---

void TypeChecker::visit(const AST::Literal& expr) {
    if (expr.value.type == TokenType::IntLiteral) last_type = Type::Int;
    else if (expr.value.type == TokenType::True) last_type = Type::Bool;
    else if (expr.value.type == TokenType::False) last_type = Type::Bool;
    else last_type = Type::Error;
}

void TypeChecker::visit(const AST::Variable& expr) {
    const Symbol* symbol = resolve(expr.name);
    if (symbol) {
        last_type = symbol->type;
    } else {
        std::cerr << "Semantic Error: Undefined variable '" << expr.name.lexeme << "'.\n";
        had_error = true;
        last_type = Type::Error;
    }
}

void TypeChecker::visit(const AST::Assign& expr) {
    Type value_type = type_of(*expr.value);
    const Symbol* symbol = resolve(expr.name);

    if (symbol) {
        if (!symbol->is_mutable) {
            std::cerr << "Semantic Error: Cannot assign to immutable variable '" << expr.name.lexeme << "'.\n";
            had_error = true;
        }
        check_type(symbol->type, value_type, "Type mismatch in assignment.");
        last_type = value_type;
    } else {
        std::cerr << "Semantic Error: Assignment to undeclared variable '" << expr.name.lexeme << "'.\n";
        had_error = true;
        last_type = Type::Error;
    }
}

void TypeChecker::visit(const AST::Call& expr) {
    // We can only get the type of a function call if the callee is a simple variable.
    if (const auto* var = dynamic_cast<const AST::Variable*>(expr.callee.get())) {
        const Symbol* symbol = resolve(var->name);
        if (symbol) {
            // For now, we assume all callable things are functions and return their declared type.
            // A full implementation would handle function types, arity, etc.
            last_type = symbol->type;
            return;
        }
    }
    
    std::cerr << "Semantic Error: Cannot determine type of complex callee.\n";
    had_error = true;
    last_type = Type::Error;
}


void TypeChecker::visit(const AST::Unary& expr) {
    Type right_type = type_of(*expr.right);
    if (expr.op.type == TokenType::Minus) {
        check_type(Type::Int, right_type, "Operand for unary minus must be an integer.");
        last_type = Type::Int;
    } else if (expr.op.type == TokenType::Bang) {
        check_type(Type::Bool, right_type, "Operand for logical not must be a boolean.");
        last_type = Type::Bool;
    } else {
        last_type = Type::Error;
    }
}

void TypeChecker::visit(const AST::Binary& expr) {
    Type left_type = type_of(*expr.left);
    Type right_type = type_of(*expr.right);

    switch (expr.op.type) {
        case TokenType::Plus:
        case TokenType::Minus:
        case TokenType::Star:
        case TokenType::Slash:
            check_type(Type::Int, left_type, "Left operand for arithmetic must be an integer.");
            check_type(Type::Int, right_type, "Right operand for arithmetic must be an integer.");
            last_type = Type::Int;
            break;
        case TokenType::Greater:
        case TokenType::GreaterEqual:
        case TokenType::Less:
        case TokenType::LessEqual:
            check_type(Type::Int, left_type, "Left operand for comparison must be an integer.");
            check_type(Type::Int, right_type, "Right operand for comparison must be an integer.");
            last_type = Type::Bool;
            break;
        case TokenType::EqualEqual:
        case TokenType::BangEqual:
            check_type(left_type, right_type, "Type mismatch in equality comparison.");
            last_type = Type::Bool;
            break;
        default:
            last_type = Type::Error;
            break;
    }
}

} // namespace Quastra
