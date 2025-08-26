#include "codegen.hpp"

namespace Quastra {

std::string CodeGen::generate(const std::vector<std::unique_ptr<AST::Stmt>>& statements) {
    // Add standard C++ includes.
    output << "#include <iostream>\n";
    output << "#include <vector>\n\n";

    // Generate code for each top-level statement (now including functions).
    for (const auto& stmt : statements) {
        if (stmt) {
            generate_code(*stmt);
        }
    }

    return output.str();
}

// --- Visitor Implementations ---

void CodeGen::indent() {
    for (int i = 0; i < indent_level; ++i) {
        output << "    ";
    }
}

void CodeGen::visit(const AST::ExprStmt& stmt) {
    indent();
    generate_code(*stmt.expression);
    output << ";\n";
}

void CodeGen::visit(const AST::VarDecl& stmt) {
    indent();
    output << "auto " << stmt.name.lexeme << " = ";
    if (stmt.initializer) {
        generate_code(*stmt.initializer);
    } else {
        output << "0"; // Default initialize
    }
    output << ";\n";
}

void CodeGen::visit(const AST::Block& stmt) {
    output << "{\n";
    indent_level++;
    for (const auto& statement : stmt.statements) {
        generate_code(*statement);
    }
    indent_level--;
    indent();
    output << "}\n";
}

void CodeGen::visit(const AST::IfStmt& stmt) {
    indent();
    output << "if (";
    generate_code(*stmt.condition);
    output << ") ";
    generate_code(*stmt.then_branch);
    if (stmt.else_branch) {
        indent();
        output << "else ";
        generate_code(*stmt.else_branch);
    }
}

void CodeGen::visit(const AST::WhileStmt& stmt) {
    indent();
    output << "while (";
    generate_code(*stmt.condition);
    output << ") ";
    generate_code(*stmt.body);
}

void CodeGen::visit(const AST::FunctionStmt& stmt) {
    // Special case for main function to ensure correct return type.
    if (stmt.name.lexeme == "main") {
        output << "int " << stmt.name.lexeme << "(";
    } else {
        output << "auto " << stmt.name.lexeme << "(";
    }

    for (size_t i = 0; i < stmt.params.size(); ++i) {
        output << "auto " << stmt.params[i].lexeme;
        if (i < stmt.params.size() - 1) output << ", ";
    }
    output << ") ";
    generate_code(AST::Block(std::move(const_cast<std::vector<std::unique_ptr<AST::Stmt>>&>(stmt.body))));
    output << "\n";
}

void CodeGen::visit(const AST::ReturnStmt& stmt) {
    indent();
    output << "return ";
    if (stmt.value) {
        generate_code(*stmt.value);
    }
    output << ";\n";
}

void CodeGen::visit(const AST::Literal& expr) {
    output << expr.value.lexeme;
}

void CodeGen::visit(const AST::Variable& expr) {
    output << expr.name.lexeme;
}

void CodeGen::visit(const AST::Assign& expr) {
    output << "(" << expr.name.lexeme << " = ";
    generate_code(*expr.value);
    output << ")";
}

void CodeGen::visit(const AST::Unary& expr) {
    output << "(" << expr.op.lexeme;
    generate_code(*expr.right);
    output << ")";
}

void CodeGen::visit(const AST::Binary& expr) {
    output << "(";
    generate_code(*expr.left);
    output << " " << expr.op.lexeme << " ";
    generate_code(*expr.right);
    output << ")";
}

void CodeGen::visit(const AST::Call& expr) {
    generate_code(*expr.callee);
    output << "(";
    for (size_t i = 0; i < expr.arguments.size(); ++i) {
        generate_code(*expr.arguments[i]);
        if (i < expr.arguments.size() - 1) output << ", ";
    }
    output << ")";
}


// --- Helper Methods ---

void CodeGen::generate_code(const AST::Stmt& stmt) {
    stmt.accept(*this);
}

void CodeGen::generate_code(const AST::Expr& expr) {
    expr.accept(*this);
}

} // namespace Quastra
