// Comprehensive Quastra Example
// This program demonstrates all features currently supported by the compiler.

// A recursive function to calculate the nth Fibonacci number.
fn fib(n) {
    if (n < 2) {
        return n;
    }
    // Note: Our interpreter can run this, but the generated C++ will need
    // forward declarations or a different ordering to compile.
    // This is a feature for a future semantic analysis pass.
    let result = fib(n - 2) + fib(n - 1);
    return result;
}

// The main entry point of the program.
fn main() {
   
    let mut counter = 0;
    let limit = 5;

    // Use a while loop to iterate.
    while (counter < limit) {
        // A nested block to demonstrate scope.
        {
            let counter = 99; // Shadow the outer 'counter'.
            // This assignment only affects the inner 'counter'.
            counter = counter + 1;
        }

        // Use an if-else statement for conditional logic.
        if (counter == 3) {
            let message = 1; // Placeholder for a string type
        } else {
            let message = 0;
        }

        counter = counter + 1;
    }

    // Call the recursive function after the loop.
    let fib_result = fib(7); // fib(7) is 13

    // Final assignment using the result.
    let final_value = fib_result + counter; // Should be 13 + 5 = 18

    return 0;
}
