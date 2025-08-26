#pragma once

#include "quastra_callable.hpp"
#include "quastra_value.hpp"
#include <iostream>
#include <vector>

namespace Quastra {

// A native C++ implementation of a "println" function.
class PrintlnFunction : public QuastraCallable {
public:
    // println takes one argument.
    int arity() const override { return 1; }

    // The core logic that gets executed when the function is called.
    QuastraValue call(Interpreter& interpreter, const std::vector<QuastraValue>& arguments) override {
        (void)interpreter; // Interpreter is unused in this simple function.
        print_value(arguments[0]);
        std::cout << std::endl;
        return false; // println returns nothing (represented as false for now).
    }
};

} // namespace Quastra
