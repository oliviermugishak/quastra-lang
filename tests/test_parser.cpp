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

    void visit(const AST::Block& stmt) override {
        result += "{ ";
        for (const auto& statement : stmt.statements) {
            statement->accept(*this);
        }
        result += "} ";
    }

    void visit(const AST::IfStmt& stmt) override {
        result += "(if ";
        stmt.condition->accept(*this);
        result += " ";
        stmt.then_branch->accept(*this);
        if (stmt.else_branch) {
            result += "else ";
            stmt.else_branch->accept(*this);
        }
        result += ") ";
    }

    void visit(const AST::WhileStmt& stmt) override {
        result += "(while ";
        stmt.condition->accept(*this);
        result += " ";
        stmt.body->accept(*this);
        result += ") ";
    }

    void visit(const AST::FunctionStmt& stmt) override {
        result += "(fn-decl " + stmt.name.lexeme + "() ";
        result += "{ ";
        for (const auto& statement : stmt.body) {
            statement->accept(*this);
        }
        result += "} ";
        result += ") ";
    }

    void visit(const AST::ReturnStmt& stmt) override {
        result += "(return ";
        if (stmt.value) {
            stmt.value->accept(*this);
        }
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

    void visit(const AST::Variable& expr) override {
        result += expr.name.lexeme;
    }

    void visit(const AST::Assign& expr) override {
        result += "(" + expr.name.lexeme + " = ";
        expr.value->accept(*this);
        result += ")";
    }

    void visit(const AST::Call& expr) override {
        result += "(call ";
        expr.callee->accept(*this);
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

TEST(ParserStatementTest, ParseAssignment) {
    std::string source = "x = 20;";
    std::string expected = "(expr-stmt (x = 20);) ";
    EXPECT_EQ(parse_and_print(source), expected);
}

TEST(ParserStatementTest, ParseBlockStatement) {
    std::string source = "{ let a = 1; a = 2; }";
    std::string expected = "{ (var-decl a = 1;) (expr-stmt (a = 2);) } ";
    EXPECT_EQ(parse_and_print(source), expected);
}

TEST(ParserControlFlowTest, ParseIfStatement) {
    std::string source = "if (x > 1) y = 1;";
    std::string expected = "(if (x > 1) (expr-stmt (y = 1);) ) ";
    EXPECT_EQ(parse_and_print(source), expected);
}

TEST(ParserControlFlowTest, ParseIfElseStatement) {
    std::string source = "if (x == 1) y = 1; else y = 2;";
    std::string expected = "(if (x == 1) (expr-stmt (y = 1);) else (expr-stmt (y = 2);) ) ";
    EXPECT_EQ(parse_and_print(source), expected);
}

TEST(ParserControlFlowTest, ParseWhileStatement) {
    std::string source = "while (x < 10) x = x + 1;";
    std::string expected = "(while (x < 10) (expr-stmt (x = (x + 1));) ) ";
    EXPECT_EQ(parse_and_print(source), expected);
}

TEST(ParserFunctionTest, ParseFunctionDeclaration) {
    std::string source = "fn my_func() { return 1; }";
    std::string expected = "(fn-decl my_func() { (return 1;) } ) ";
    EXPECT_EQ(parse_and_print(source), expected);
}

TEST(ParserFunctionTest, ParseFunctionCall) {
    std::string source = "my_func();";
    std::string expected = "(expr-stmt (call my_func);) ";
    EXPECT_EQ(parse_and_print(source), expected);
}
