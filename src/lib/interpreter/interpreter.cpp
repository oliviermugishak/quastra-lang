#include "interpreter.hpp"
#include <stdexcept>

namespace Quastra {

// Helper function to determine if a QuastraValue is "truthy".
// In Quastra, `false` is false, everything else is true.
bool is_truthy(const QuastraValue& value) {
    if (std::holds_alternative<bool>(value)) {
        return std::get<bool>(value);
    }
    // All other types (numbers, strings) are considered true.
    return true;
}

// Helper to check for equality between two QuastraValues.
bool is_equal(const QuastraValue& a, const QuastraValue& b) {
    return a == b;
}

Interpreter::Interpreter() {
    environment = std::make_shared<Environment>();
}

void Interpreter::interpret(const std::vector<std::unique_ptr<AST::Stmt>>& statements) {
    try {
        for (const auto& statement : statements) {
            if (statement) {
                statement->accept(*this);
            }
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
    QuastraValue value = false; // Default initialize to false
    if (stmt.initializer) {
        value = evaluate(*stmt.initializer);
    }
    environment->define(stmt.name.lexeme, value);
}

void Interpreter::visit(const AST::Block& stmt) {
    // Create a new environment for the block's scope.
    auto new_env = std::make_shared<Environment>(environment);
    // Temporarily swap environments
    auto old_env = environment;
    environment = new_env;

    for (const auto& statement : stmt.statements) {
        statement->accept(*this);
    }

    // Restore the previous environment.
    environment = old_env;
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


// --- Expression Evaluation ---

QuastraValue Interpreter::evaluate(const AST::Expr& expr) {
    expr.accept(*this);
    return last_evaluated_value;
}

void Interpreter::visit(const AST::Literal& expr) {
    if (expr.value.type == TokenType::IntLiteral) {
        last_evaluated_value = std::stod(expr.value.lexeme);
    } else if (expr.value.type == TokenType::True) {
        last_evaluated_value = true;
    } else if (expr.value.type == TokenType::False) {
        last_evaluated_value = false;
    } else {
        last_evaluated_value = false; // Default
    }
}

void Interpreter::visit(const AST::Variable& expr) {
    last_evaluated_value = environment->get(expr.name);
}

void Interpreter::visit(const AST::Assign& expr) {
    QuastraValue value = evaluate(*expr.value);
    environment->assign(expr.name, value);
    last_evaluated_value = value;
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
        // Comparisons
        case TokenType::EqualEqual:
            last_evaluated_value = is_equal(left, right);
            return;
        case TokenType::BangEqual:
            last_evaluated_value = !is_equal(left, right);
            return;
        case TokenType::Greater:
            if (std::holds_alternative<double>(left) && std::holds_alternative<double>(right)) {
                last_evaluated_value = std::get<double>(left) > std::get<double>(right);
                return;
            }
            throw std::runtime_error("Operands must be numbers for comparison.");
        case TokenType::GreaterEqual:
            if (std::holds_alternative<double>(left) && std::holds_alternative<double>(right)) {
                last_evaluated_value = std::get<double>(left) >= std::get<double>(right);
                return;
            }
            throw std::runtime_error("Operands must be numbers for comparison.");
        case TokenType::Less:
            if (std::holds_alternative<double>(left) && std::holds_alternative<double>(right)) {
                last_evaluated_value = std::get<double>(left) < std::get<double>(right);
                return;
            }
            throw std::runtime_error("Operands must be numbers for comparison.");
        case TokenType::LessEqual:
            if (std::holds_alternative<double>(left) && std::holds_alternative<double>(right)) {
                last_evaluated_value = std::get<double>(left) <= std::get<double>(right);
                return;
            }
            throw std::runtime_error("Operands must be numbers for comparison.");

        // Arithmetic
        case TokenType::Plus:
            if (std::holds_alternative<double>(left) && std::holds_alternative<double>(right)) {
                last_evaluated_value = std::get<double>(left) + std::get<double>(right);
                return;
            }
            throw std::runtime_error("Operands must be numbers for addition.");
        case TokenType::Minus:
            if (std::holds_alternative<double>(left) && std::holds_alternative<double>(right)) {
                last_evaluated_value = std::get<double>(left) - std::get<double>(right);
                return;
            }
            throw std::runtime_error("Operands must be numbers for subtraction.");
        case TokenType::Star:
            if (std::holds_alternative<double>(left) && std::holds_alternative<double>(right)) {
                last_evaluated_value = std::get<double>(left) * std::get<double>(right);
                return;
            }
            throw std::runtime_error("Operands must be numbers for multiplication.");
        case TokenType::Slash:
            if (std::holds_alternative<double>(left) && std::holds_alternative<double>(right)) {
                if (std::get<double>(right) == 0) throw std::runtime_error("Division by zero.");
                last_evaluated_value = std::get<double>(left) / std::get<double>(right);
                return;
            }
            throw std::runtime_error("Operands must be numbers for division.");
        default:
            break;
    }
    throw std::runtime_error("Invalid binary operation.");
}

} // namespace Quastra
