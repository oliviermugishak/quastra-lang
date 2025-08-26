#pragma once

#include <variant>
#include <string>
#include <iostream>

namespace Quastra {

// A variant-based class to represent any possible value in Quastra at runtime.
using QuastraValue = std::variant<double, bool, std::string>;

// Helper function to print a QuastraValue, useful for debugging.
inline void print_value(const QuastraValue& value) {
    std::visit([](const auto& arg) {
        if constexpr (std::is_same_v<decltype(arg), const bool&>) {
            std::cout << (arg ? "true" : "false");
        } else {
            std::cout << arg;
        }
    }, value);
}

} // namespace Quastra
