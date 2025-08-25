// file: src/codegen.c
#include "codegen.h"
#include "common.h"

// We are no longer using a dynamic string buffer.
// Instead, we are writing directly to a file stream.

// Function prototypes for our new approach.
static void generate_expression(ASTNode* node, FILE* output);
static void generate_statement(ASTNode* node, FILE* output);
static void generate_var_declaration(ASTNode* node, FILE* output);
static void generate_return_statement(ASTNode* node, FILE* output);
static void generate_block(ASTNode* node, FILE* output);

// Generates C++ code for a token's lexeme and writes it to the file.
static void generate_token_lexeme(const Token* token, FILE* output) {
    fprintf(output, "%.*s", token->length, token->lexeme);
}

// Generates C++ code for a literal expression (e.g., 42, "hello").
static void generate_literal_expression(ASTNode* node, FILE* output) {
    generate_token_lexeme(&node->literal_expr.literal, output);
}

// Generates C++ code for a binary expression (e.g., 1 + 2).
static void generate_binary_expression(ASTNode* node, FILE* output) {
    generate_expression(node->binary_expr.left, output);
    fprintf(output, " ");
    generate_token_lexeme(&node->binary_expr.op, output);
    fprintf(output, " ");
    generate_expression(node->binary_expr.right, output);
}

// Generates C++ code for an identifier expression (e.g., `x`).
static void generate_identifier_expression(ASTNode* node, FILE* output) {
    generate_token_lexeme(&node->identifier_expr.identifier, output);
}

// Generates C++ code for an expression.
static void generate_expression(ASTNode* node, FILE* output) {
    if (node == NULL) return;

    switch (node->type) {
        case NODE_LITERAL_EXPR:
            generate_literal_expression(node, output);
            break;
        case NODE_BINARY_EXPR:
            generate_binary_expression(node, output);
            break;
        case NODE_IDENTIFIER_EXPR:
            generate_identifier_expression(node, output);
            break;
        default:
            fprintf(stderr, "Error: Unhandled expression type in codegen: %d\n", node->type);
            exit(1);
    }
}

// Generates C++ code for a variable declaration.
static void generate_var_declaration(ASTNode* node, FILE* output) {
    // For now, assume all variables are integers.
    fprintf(output, "  int ");
    generate_token_lexeme(&node->var_decl.name, output);
    fprintf(output, " = ");
    generate_expression(node->var_decl.initializer, output);
    fprintf(output, ";\n");
}

// Generates C++ code for a return statement.
static void generate_return_statement(ASTNode* node, FILE* output) {
    fprintf(output, "  return ");
    generate_expression(node->return_stmt.value, output);
    fprintf(output, ";\n");
}

// Generates C++ code for a block of statements.
static void generate_block(ASTNode* node, FILE* output) {
    for (int i = 0; i < node->block.count; i++) {
        generate_statement(node->block.statements[i], output);
    }
}

// Generates C++ code for a statement.
static void generate_statement(ASTNode* node, FILE* output) {
    if (node == NULL) return;
    
    switch (node->type) {
        case NODE_VAR_DECL:
            generate_var_declaration(node, output);
            break;
        case NODE_RETURN_STMT:
            generate_return_statement(node, output);
            break;
        case NODE_BLOCK:
            generate_block(node, output);
            break;
        default:
            fprintf(stderr, "Error: Unhandled statement type in codegen: %d\n", node->type);
            exit(1);
    }
}

// Generates C++ code for a function declaration.
static void generate_fn_declaration(ASTNode* node, FILE* output) {
    // For now, we hardcode the return type as `int`.
    fprintf(output, "int ");
    generate_token_lexeme(&node->fn_decl.name, output);
    fprintf(output, "() {\n");
    generate_statement(node->fn_decl.body, output);
    fprintf(output, "}\n");
}

// The main function for code generation.
void quastra_codegen(ASTNode* root, FILE* output) {
    // Add C++ boilerplate
    fprintf(output, "#include <iostream>\n");
    fprintf(output, "\n");
    
    // Generate code for the main function.
    if (root->type == NODE_FN_DECL) {
        generate_fn_declaration(root, output);
    }
}
