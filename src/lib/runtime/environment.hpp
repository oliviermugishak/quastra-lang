#pragma once

#include "quastra_value.hpp"
#include "../frontend/token.hpp"
#include <map>
#include <string>
#include <memory>

namespace Quastra {

// Manages the state of variables, including scopes.
class Environment {
public:
    // Create a global scope.
    Environment() : enclosing(nullptr) {}
    // Create a nested (local) scope.
    Environment(std::shared_ptr<Environment> enclosing) : enclosing(enclosing) {}

    // Define a new variable in the current scope.
    void define(const std::string& name, const QuastraValue& value) {
        values[name] = value;
    }

    // Assign a new value to an existing variable.
    void assign(const Token& name, const QuastraValue& value) {
        if (values.count(name.lexeme)) {
            values[name.lexeme] = value;
            return;
        }

        if (enclosing != nullptr) {
            enclosing->assign(name, value);
            return;
        }

        throw std::runtime_error("Undefined variable '" + name.lexeme + "'.");
    }

    // Get the value of a variable.
    QuastraValue get(const Token& name) {
        if (values.count(name.lexeme)) {
            return values.at(name.lexeme);
        }
        // If not in current scope, check the parent scope.
        if (enclosing != nullptr) {
            return enclosing->get(name);
        }
        throw std::runtime_error("Undefined variable '" + name.lexeme + "'.");
    }

private:
    std::map<std::string, QuastraValue> values;
    std::shared_ptr<Environment> enclosing;
};

} // namespace Quastra
