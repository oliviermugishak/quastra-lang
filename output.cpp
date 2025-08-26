#include <iostream>
#include <vector>

auto fib(auto n) {
    if ((n < 2)) {
        return n;
    }
    auto result = (fib((n - 2)) + fib((n - 1)));
    return result;
}

int main() {
    auto counter = 0;
    auto limit = 5;
    while ((counter < limit)) {
        {
            auto counter = 99;
            (counter = (counter + 1));
        }
        
        if ((counter == 3)) {
            auto message = 1;
        }
        else {
            auto message = 0;
        }
        (counter = (counter + 1));
    }
    auto fib_result = fib(7);
    auto final_value = (fib_result + counter);
    std::cout << "Result of count(): " << std::to_string(final_value) << std::endl;
    return 0;
}

