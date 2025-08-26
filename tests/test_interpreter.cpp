#include <gtest/gtest.h>
#include "lib/frontend/lexer.hpp"
#include "lib/frontend/parser.hpp"
#include "lib/interpreter/interpreter.hpp"
#include "lib/runtime/environment.hpp"
#include <variant>

using namespace Quastra;

// Helper to interpret code and get the final environment.
std::shared_ptr<Environment> interpret_and_get_env(const std::string& source) {
    Lexer lexer(source);
    auto tokens = lexer.scan_tokens();
    Parser parser(tokens);
    auto statements = parser.parse();
    auto interpreter = std::make_shared<Interpreter>();
    interpreter->interpret(statements);
    return interpreter->get_environment();
}

// Helper to interpret code and get the value of the last expression.
QuastraValue interpret_and_get_value(const std::string& source) {
    class TestInterpreter : public Interpreter {
    public:
        QuastraValue get_last_value() { return last_evaluated_value; }
    };
    Lexer lexer(source);
    auto tokens = lexer.scan_tokens();
    Parser parser(tokens);
    auto statements = parser.parse();
    auto interpreter = std::make_shared<TestInterpreter>();
    interpreter->interpret(statements);
    return interpreter->get_last_value();
}

TEST(InterpreterControlFlowTest, IfStatementTrue) {
    std::string source = "let x = 0; if (true) { x = 1; }";
    auto env = interpret_and_get_env(source);
    EXPECT_EQ(std::get<double>(env->get({TokenType::Identifier, "x", 1})), 1.0);
}

TEST(InterpreterControlFlowTest, IfStatementFalse) {
    std::string source = "let x = 0; if (false) { x = 1; }";
    auto env = interpret_and_get_env(source);
    EXPECT_EQ(std::get<double>(env->get({TokenType::Identifier, "x", 1})), 0.0);
}

TEST(InterpreterControlFlowTest, IfElseStatement) {
    std::string source = "let x = 0; if (1 > 2) { x = 1; } else { x = 2; }";
    auto env = interpret_and_get_env(source);
    EXPECT_EQ(std::get<double>(env->get({TokenType::Identifier, "x", 1})), 2.0);
}

TEST(InterpreterControlFlowTest, WhileLoop) {
    std::string source = "let x = 0; while (x < 3) { x = x + 1; }";
    auto env = interpret_and_get_env(source);
    EXPECT_EQ(std::get<double>(env->get({TokenType::Identifier, "x", 1})), 3.0);
}

TEST(InterpreterControlFlowTest, BlockScoping) {
    std::string source = "let a = 1; { let a = 2; }";
    auto env = interpret_and_get_env(source);
    // 'a' should still be 1 in the outer scope.
    EXPECT_EQ(std::get<double>(env->get({TokenType::Identifier, "a", 1})), 1.0);
}

TEST(InterpreterBooleanTest, ComparisonOperators) {
    EXPECT_EQ(std::get<bool>(interpret_and_get_value("1 < 2;")), true);
    EXPECT_EQ(std::get<bool>(interpret_and_get_value("1 > 2;")), false);
    EXPECT_EQ(std::get<bool>(interpret_and_get_value("2 >= 2;")), true);
    EXPECT_EQ(std::get<bool>(interpret_and_get_value("2 <= 2;")), true);
    EXPECT_EQ(std::get<bool>(interpret_and_get_value("5 == 5;")), true);
    EXPECT_EQ(std::get<bool>(interpret_and_get_value("5 != 5;")), false);
}
