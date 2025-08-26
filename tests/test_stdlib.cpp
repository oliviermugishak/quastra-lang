#include <gtest/gtest.h>
#include "lib/frontend/lexer.hpp"
#include "lib/frontend/parser.hpp"
#include "lib/interpreter/interpreter.hpp"
#include <string>
#include <iostream>
#include <streambuf>

using namespace Quastra;

TEST(StdLibTest, PrintlnFunction) {
    std::string source = "println(123);";

    // This is a common C++ technique to capture std::cout for testing.
    std::streambuf* old_cout_streambuf = std::cout.rdbuf();
    std::ostringstream captured_output;
    std::cout.rdbuf(captured_output.rdbuf());

    // Interpret the code, which should call our native println.
    Lexer lexer(source);
    auto tokens = lexer.scan_tokens();
    Parser parser(tokens);
    auto statements = parser.parse();
    Interpreter interpreter;
    interpreter.interpret(statements);

    // Restore the original std::cout.
    std::cout.rdbuf(old_cout_streambuf);

    // Verify that the captured output is correct.
    ASSERT_EQ(captured_output.str(), "123\n");
}
