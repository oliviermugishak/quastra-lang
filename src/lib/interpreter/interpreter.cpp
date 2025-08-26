#include "interpreter.hpp"
#include "../runtime/quastra_callable.hpp"
#include "../runtime/native_functions.hpp" // Include our new native function
#include <stdexcept>

namespace Quastra {

// Helper functions (as before)
bool is_truthy(const QuastraValue& value) {
    if (std::holds_alternative<bool>(value)) return std::get<bool>(value);
    return true;
}
bool is_equal(const QuastraValue& a, const QuastraValue& b) { return a == b; }

Interpreter::Interpreter() {
    environment = std::make_shared<Environment>();
    // Define the native println function in the global scope.
    environment->define("println", std::make_shared<PrintlnFunction>());
}

void Interpreter::interpret(const std::vector<std::unique_ptr<AST::Stmt>>& statements) {
    try {
        for (const auto& statement : statements) {
            if (statement) statement->accept(*this);
        }
    } catch (const std::runtime_error& error) {
        std::cerr << "Runtime Error: " << error.what() << std::endl;
    }
}

// --- Statement Execution ---

void Interpreter::visit(const AST::ExprStmt& stmt) {
    last_evaluated_value = evaluate(*stmt.expression);
}

void Interpreter::visit(const AST::VarDecl& stmt) {
    QuastraValue value = false;
    if (stmt.initializer) value = evaluate(*stmt.initializer);
    environment->define(stmt.name.lexeme, value);
}

void Interpreter::visit(const AST::Block& stmt) {
    execute_block(stmt.statements, std::make_shared<Environment>(environment));
}

// Public method to execute a block in a specific environment.
void Interpreter::execute_block(const std::vector<std::unique_ptr<AST::Stmt>>& statements, std::shared_ptr<Environment> block_environment) {
    auto previous = this->environment;
    try {
        this->environment = block_environment;
        for (const auto& statement : statements) {
            statement->accept(*this);
        }
    } catch (...) {
        this->environment = previous; // Ensure environment is restored on exception
        throw;
    }
    this->environment = previous;
}

void Interpreter::visit(const AST::IfStmt& stmt) {
    if (is_truthy(evaluate(*stmt.condition))) {
        stmt.then_branch->accept(*this);
    } else if (stmt.else_branch) {
        stmt.else_branch->accept(*this);
    }
}

void Interpreter::visit(const AST::WhileStmt& stmt) {
    while (is_truthy(evaluate(*stmt.condition))) {
        stmt.body->accept(*this);
    }
}

void Interpreter::visit(const AST::FunctionStmt& stmt) {
    auto function = std::make_shared<QuastraFunction>(stmt, environment);
    environment->define(stmt.name.lexeme, function);
}

void Interpreter::visit(const AST::ReturnStmt& stmt) {
    QuastraValue value = false;
    if (stmt.value) {
        value = evaluate(*stmt.value);
    }
    throw ReturnException(value);
}

// --- Expression Evaluation ---

QuastraValue Interpreter::evaluate(const AST::Expr& expr) {
    expr.accept(*this);
    return last_evaluated_value;
}

void Interpreter::visit(const AST::Literal& expr) {
    if (expr.value.type == TokenType::IntLiteral) last_evaluated_value = std::stod(expr.value.lexeme);
    else if (expr.value.type == TokenType::True) last_evaluated_value = true;
    else if (expr.value.type == TokenType::False) last_evaluated_value = false;
    else last_evaluated_value = false;
}

void Interpreter::visit(const AST::Variable& expr) {
    last_evaluated_value = environment->get(expr.name);
}

void Interpreter::visit(const AST::Assign& expr) {
    QuastraValue value = evaluate(*expr.value);
    environment->assign(expr.name, value);
    last_evaluated_value = value;
}

void Interpreter::visit(const AST::Call& expr) {
    QuastraValue callee = evaluate(*expr.callee);

    if (!std::holds_alternative<std::shared_ptr<QuastraCallable>>(callee)) {
        throw std::runtime_error("Can only call functions and classes.");
    }
    auto function = std::get<std::shared_ptr<QuastraCallable>>(callee);

    if (expr.arguments.size() != static_cast<size_t>(function->arity())) {
        throw std::runtime_error("Expected " + std::to_string(function->arity()) + " arguments but got " + std::to_string(expr.arguments.size()) + ".");
    }

    std::vector<QuastraValue> arguments;
    for (const auto& arg_expr : expr.arguments) {
        arguments.push_back(evaluate(*arg_expr));
    }

    try {
        last_evaluated_value = function->call(*this, arguments);
    } catch (const ReturnException& returnValue) {
        last_evaluated_value = returnValue.value;
    }
}

void Interpreter::visit(const AST::Unary& expr) {
    QuastraValue right = evaluate(*expr.right);
    if (expr.op.type == TokenType::Minus) {
        if (std::holds_alternative<double>(right)) {
            last_evaluated_value = -std::get<double>(right);
            return;
        }
        throw std::runtime_error("Operand must be a number for unary minus.");
    }
    if (expr.op.type == TokenType::Bang) {
        last_evaluated_value = !is_truthy(right);
        return;
    }
}

void Interpreter::visit(const AST::Binary& expr) {
    QuastraValue left = evaluate(*expr.left);
    QuastraValue right = evaluate(*expr.right);

    switch (expr.op.type) {
        case TokenType::EqualEqual: last_evaluated_value = is_equal(left, right); return;
        case TokenType::BangEqual: last_evaluated_value = !is_equal(left, right); return;
        case TokenType::Greater:
            if (std::holds_alternative<double>(left) && std::holds_alternative<double>(right)) {
                last_evaluated_value = std::get<double>(left) > std::get<double>(right); return;
            } throw std::runtime_error("Operands must be numbers for comparison.");
        case TokenType::GreaterEqual:
            if (std::holds_alternative<double>(left) && std::holds_alternative<double>(right)) {
                last_evaluated_value = std::get<double>(left) >= std::get<double>(right); return;
            } throw std::runtime_error("Operands must be numbers for comparison.");
        case TokenType::Less:
            if (std::holds_alternative<double>(left) && std::holds_alternative<double>(right)) {
                last_evaluated_value = std::get<double>(left) < std::get<double>(right); return;
            } throw std::runtime_error("Operands must be numbers for comparison.");
        case TokenType::LessEqual:
            if (std::holds_alternative<double>(left) && std::holds_alternative<double>(right)) {
                last_evaluated_value = std::get<double>(left) <= std::get<double>(right); return;
            } throw std::runtime_error("Operands must be numbers for comparison.");
        case TokenType::Plus:
            if (std::holds_alternative<double>(left) && std::holds_alternative<double>(right)) {
                last_evaluated_value = std::get<double>(left) + std::get<double>(right); return;
            } throw std::runtime_error("Operands must be numbers for addition.");
        case TokenType::Minus:
            if (std::holds_alternative<double>(left) && std::holds_alternative<double>(right)) {
                last_evaluated_value = std::get<double>(left) - std::get<double>(right); return;
            } throw std::runtime_error("Operands must be numbers for subtraction.");
        case TokenType::Star:
            if (std::holds_alternative<double>(left) && std::holds_alternative<double>(right)) {
                last_evaluated_value = std::get<double>(left) * std::get<double>(right); return;
            } throw std::runtime_error("Operands must be numbers for multiplication.");
        case TokenType::Slash:
            if (std::holds_alternative<double>(left) && std::holds_alternative<double>(right)) {
                if (std::get<double>(right) == 0) throw std::runtime_error("Division by zero.");
                last_evaluated_value = std::get<double>(left) / std::get<double>(right); return;
            } throw std::runtime_error("Operands must be numbers for division.");
        default: break;
    }
    throw std::runtime_error("Invalid binary operation.");
}

} // namespace Quastra
