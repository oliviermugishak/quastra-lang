#include <gtest/gtest.h>
#include "lib/frontend/lexer.hpp"
#include "lib/frontend/parser.hpp"
#include "lib/backend/codegen.hpp"
#include <string>

using namespace Quastra;

// Helper function to run the full lex->parse->generate pipeline.
static std::string generate_cpp(const std::string& source) {
    Lexer lexer(source);
    auto tokens = lexer.scan_tokens();
    Parser parser(tokens);
    auto statements = parser.parse();
    
    CodeGen codegen;
    return codegen.generate(statements);
}

TEST(CodeGenTest, GeneratesWhileLoop) {
    std::string source = "let i = 0; while (i < 5) { i = i + 1; }";
    std::string wrapped_source = "fn main() { " + source + " return 0; }";

    std::string expected_cpp =
R"(#include <iostream>
#include <vector>

int main() {
    auto i = 0;
    while ((i < 5)) {
        (i = (i + 1));
    }
    return 0;
}

)";
    ASSERT_EQ(generate_cpp(wrapped_source), expected_cpp);
}

TEST(CodeGenFunctionTest, GeneratesFunctions) {
    std::string source = R"(
fn add(a, b) {
    return a + b;
}

fn main() {
    let result = add(5, 3);
    return 0;
}
)";
    // CORRECTED: Added the final newline to match the generator's output.
    std::string expected_cpp =
R"(#include <iostream>
#include <vector>

auto add(auto a, auto b) {
    return (a + b);
}

int main() {
    auto result = add(5, 3);
    return 0;
}

)";
    ASSERT_EQ(generate_cpp(source), expected_cpp);
}
