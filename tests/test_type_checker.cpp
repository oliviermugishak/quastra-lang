#include <gtest/gtest.h>
#include "lib/frontend/lexer.hpp"
#include "lib/frontend/parser.hpp"
#include "lib/semantic/type_checker.hpp"
#include <string>

using namespace Quastra;

// Helper function to run the full lex->parse->check pipeline.
// Returns true if the type checker found no errors.
static bool type_check(const std::string& source) {
    Lexer lexer(source);
    auto tokens = lexer.scan_tokens();
    Parser parser(tokens);
    auto statements = parser.parse();
    
    TypeChecker checker;
    return checker.check(statements);
}

TEST(TypeCheckerTest, ValidProgram) {
    std::string source = R"(
        fn add(a, b) {
            return a + b;
        }
        let mut x = 10;
        if (x > 5) {
            x = add(x, 1);
        }
    )";
    ASSERT_TRUE(type_check(source));
}

TEST(TypeCheckerTest, ErrorAssignToImmutable) {
    std::string source = "let x = 10; x = 20;";
    ASSERT_FALSE(type_check(source));
}

TEST(TypeCheckerTest, ErrorWrongReturnType) {
    std::string source = "fn my_func() { return true; }";
    ASSERT_FALSE(type_check(source));
}

TEST(TypeCheckerTest, ErrorReturnFromTopLevel) {
    std::string source = "return 10;";
    ASSERT_FALSE(type_check(source));
}
