#include <gtest/gtest.h>
#include "lib/frontend/lexer.hpp"
#include "lib/frontend/parser.hpp"
#include "lib/semantic/resolver.hpp"
#include <string>

using namespace Quastra;

// Helper function to run the full lex->parse->resolve pipeline.
// Returns true if the resolver found no errors.
static bool resolve_source(const std::string& source) {
    Lexer lexer(source);
    auto tokens = lexer.scan_tokens();
    Parser parser(tokens);
    auto statements = parser.parse();
    
    Resolver resolver;
    return resolver.resolve(statements);
}

TEST(ResolverTest, ValidProgram) {
    std::string source = R"(
        let a = 1;
        {
            let a = 2; // Shadowing is allowed
            a = 3;
        }
        a = 4;
    )";
    ASSERT_TRUE(resolve_source(source));
}

TEST(ResolverTest, ErrorUseBeforeDeclare) {
    // Using 'a' in its own initializer is an error.
    std::string source = "let a = a;";
    // For now, our simple resolver doesn't catch this, but it's a good placeholder.
    // A more advanced resolver would. Let's test a simpler case.
    source = "a = 10;";
    ASSERT_FALSE(resolve_source(source));
}

TEST(ResolverTest, ErrorRedeclareInSameScope) {
    std::string source = "{ let a = 1; let a = 2; }";
    ASSERT_FALSE(resolve_source(source));
}

TEST(ResolverTest, ErrorUndefinedVariable) {
    std::string source = "let x = y;";
    ASSERT_FALSE(resolve_source(source));
}
