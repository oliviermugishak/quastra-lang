#pragma once

#include <variant>
#include <string>
#include <iostream>
#include <memory>

namespace Quastra {

class QuastraCallable; // Forward declaration

// A variant-based class to represent any possible value in Quastra at runtime.
using QuastraValue = std::variant<double, bool, std::string, std::shared_ptr<QuastraCallable>>;

// Helper function to print a QuastraValue, useful for debugging.
inline void print_value(const QuastraValue& value) {
    std::visit([](const auto& arg) {
        if constexpr (std::is_same_v<decltype(arg), const bool&>) {
            std::cout << (arg ? "true" : "false");
        } else if constexpr (std::is_same_v<decltype(arg), const std::shared_ptr<QuastraCallable>&>) {
            std::cout << "<function>";
        } else {
            std::cout << arg;
        }
    }, value);
}

} // namespace Quastra
