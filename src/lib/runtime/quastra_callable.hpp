#pragma once

#include "../interpreter/interpreter.hpp"
#include "quastra_value.hpp"
#include <vector>
#include <memory>

namespace Quastra {

// Forward declare Interpreter to avoid circular dependencies.
class Interpreter;

// An interface for any object that can be called like a function.
class QuastraCallable {
public:
    virtual ~QuastraCallable() = default;
    // The number of arguments the function expects.
    virtual int arity() const = 0;
    // The core execution logic of the function.
    virtual QuastraValue call(Interpreter& interpreter, const std::vector<QuastraValue>& arguments) = 0;
};

// A runtime representation of a Quastra function declared in the source code.
class QuastraFunction : public QuastraCallable {
public:
    QuastraFunction(const AST::FunctionStmt& declaration, std::shared_ptr<Environment> closure)
        : declaration(declaration), closure(closure) {}

    int arity() const override {
        return declaration.params.size();
    }

    QuastraValue call(Interpreter& interpreter, const std::vector<QuastraValue>& arguments) override {
        // Create a new environment for the function's execution, enclosed by the function's closure.
        auto environment = std::make_shared<Environment>(closure);
        for (size_t i = 0; i < declaration.params.size(); ++i) {
            environment->define(declaration.params[i].lexeme, arguments[i]);
        }

        // Execute the function's body in the new environment.
        interpreter.execute_block(declaration.body, environment);
        return false; // Default return value if no return statement is hit.
    }

private:
    const AST::FunctionStmt& declaration;
    std::shared_ptr<Environment> closure; // The environment where the function was declared.
};

} // namespace Quastra
