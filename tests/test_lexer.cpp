#include <gtest/gtest.h>
#include "lib/frontend/lexer.hpp"
#include <vector>
#include <string>

using namespace Quastra;

// This test fixture verifies that the lexer correctly tokenizes a simple
// main function, ignoring comments and whitespace.
TEST(LexerTest, SimpleMainFunction) {
    std::string source = R"(
// A simple function
fn main() -> int {
    return 0
}
)";

    // The ground truth: the exact sequence of tokens we expect.
    // This has been corrected to match the actual TokenType enums.
    std::vector<Token> expected_tokens = {
        {TokenType::Fn, "fn", 3},
        {TokenType::Identifier, "main", 3},
        {TokenType::LeftParen, "(", 3},
        {TokenType::RightParen, ")", 3},
        {TokenType::Arrow, "->", 3},
        {TokenType::TypeIdentifier, "int", 3},
        {TokenType::LeftBrace, "{", 3},
        {TokenType::Return, "return", 4},
        {TokenType::IntLiteral, "0", 4},
        {TokenType::RightBrace, "}", 5},
        {TokenType::EndOfFile, "", 6}
    };

    Lexer lexer(source);
    std::vector<Token> actual_tokens = lexer.scan_tokens();

    // Using Google Test's range-based comparison for cleaner output.
    ASSERT_EQ(actual_tokens.size(), expected_tokens.size());
    for (size_t i = 0; i < expected_tokens.size(); ++i) {
        EXPECT_EQ(expected_tokens[i], actual_tokens[i]) << "Mismatch at index " << i;
    }
}
