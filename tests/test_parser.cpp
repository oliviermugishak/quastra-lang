#include <gtest/gtest.h>
#include "lib/frontend/lexer.hpp"
#include "lib/frontend/parser.hpp"
#include "lib/frontend/ast.hpp"
#include <string>

using namespace Quastra;

// Helper function to run the full lex->parse pipeline.
static void parse_source(const std::string& source) {
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scan_tokens();
    Parser parser(tokens);
    auto stmts = parser.parse();
    // The act of parsing is the test itself. We just want to ensure it doesn't crash.
}

TEST(ParserErrorHandlingTest, DoesNotCrashOnMissingSemicolon) {
    std::string source = "let x = 10 let y = 20;";
    ASSERT_NO_THROW(parse_source(source));
}

TEST(ParserErrorHandlingTest, DoesNotCrashOnBadExpression) {
    std::string source = "let x = +;";
    ASSERT_NO_THROW(parse_source(source));
}

TEST(ParserErrorHandlingTest, RecoversFromMultipleErrors) {
    std::string source = R"(
        let a = 10;
        let b = * 5; // Error 1: bad expression
        let c = 30
        let d = 40;  // Error 2: missing semicolon
    )";
    ASSERT_NO_THROW(parse_source(source));
}

TEST(ParserErrorHandlingTest, DoesNotCrashOnBadStatementInBlock) {
    // This was the source of the crash. The parser needs to recover
    // from the missing semicolon inside the 'if' block.
    std::string source = "fn main() { if (true) { let x = 1 } }";
    ASSERT_NO_THROW(parse_source(source));
}
