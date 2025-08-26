#include <gtest/gtest.h>
#include "lib/frontend/lexer.hpp"
#include "lib/frontend/parser.hpp"
#include "lib/frontend/ast.hpp"
#include <string>

using namespace Quastra;

// --- AST Visitor for Testing ---
// This visitor converts a piece of the AST back into a string representation,
// which is easy to verify in tests.

class AstPrinter : public AST::ExprVisitor, public AST::StmtVisitor {
public:
    std::string print(const std::vector<std::unique_ptr<AST::Stmt>>& stmts) {
        result = "";
        for (const auto& stmt : stmts) {
            if (stmt) {
                stmt->accept(*this);
            }
        }
        return result;
    }

private:
    std::string result;

    void visit(const AST::VarDecl& stmt) override {
        result += "(var-decl " + stmt.name.lexeme;
        if (stmt.initializer) {
            result += " = ";
            stmt.initializer->accept(*this);
        }
        result += ";) ";
    }

    void visit(const AST::ExprStmt& stmt) override {
        result += "(expr-stmt ";
        stmt.expression->accept(*this);
        result += ";) ";
    }

    void visit(const AST::Literal& expr) override {
        result += expr.value.lexeme;
    }

    void visit(const AST::Unary& expr) override {
        result += "(" + expr.op.lexeme;
        expr.right->accept(*this);
        result += ")";
    }

    void visit(const AST::Binary& expr) override {
        result += "(";
        expr.left->accept(*this);
        result += " " + expr.op.lexeme + " ";
        expr.right->accept(*this);
        result += ")";
    }
};

// Helper function to run the full lex->parse->print pipeline.
std::string parse_and_print(const std::string& source) {
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scan_tokens();
    Parser parser(tokens);
    auto stmts = parser.parse();

    AstPrinter printer;
    return printer.print(stmts);
}

TEST(ParserStatementTest, ParseVariableDeclaration) {
    std::string source = "let x = 10;";
    std::string expected = "(var-decl x = 10;) ";
    EXPECT_EQ(parse_and_print(source), expected);
}

TEST(ParserStatementTest, ParseDeclarationWithExpression) {
    std::string source = "let y = 5 * 2;";
    std::string expected = "(var-decl y = (5 * 2);) ";
    EXPECT_EQ(parse_and_print(source), expected);
}

TEST(ParserStatementTest, ParseMultipleStatements) {
    std::string source = "let a = 1; let b = 2;";
    std::string expected = "(var-decl a = 1;) (var-decl b = 2;) ";
    EXPECT_EQ(parse_and_print(source), expected);
}

TEST(ParserStatementTest, ParseExpressionStatement) {
    std::string source = "1 + 1;";
    std::string expected = "(expr-stmt (1 + 1);) ";
    EXPECT_EQ(parse_and_print(source), expected);
}
TEST(ParserStatementTest, ParseComplexExpressionStatement) {
    std::string source = "3 + 4 * 5;";
    std::string expected = "(expr-stmt (3 + (4 * 5));) ";
    EXPECT_EQ(parse_and_print(source), expected);
}
TEST(ParserStatementTest, ParseUnaryExpression) {
    std::string source = "-42;";
    std::string expected = "(expr-stmt (-42);) ";
    EXPECT_EQ(parse_and_print(source), expected);
}