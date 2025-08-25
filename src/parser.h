#ifndef QUASTR_PARSER_H
#define QUASTR_PARSER_H

#include "lexer.h"

// Enum to define the type of each AST node.
typedef enum {
    NODE_FN_DECL,           // A function declaration
    NODE_VAR_DECL,          // A variable declaration (let/mut)
    NODE_RETURN_STMT,       // A return statement
    NODE_BINARY_EXPR,       // A binary expression (e.g., 1 + 2)
    NODE_LITERAL_EXPR,      // A literal value (e.g., 42, "hello")
    NODE_IDENTIFIER_EXPR,   // An identifier in an expression (e.g., `x`)
    NODE_BLOCK,             // A block of statements enclosed in {}
} ASTNodeType;

// A generic struct to hold an AST node.
// We use a union to allow different types of node data.
typedef struct ASTNode {
    ASTNodeType type;
    
    union {
        // Data for a function declaration
        struct {
            Token name;
            struct ASTNode* body; // Pointer to the body's block node
        } fn_decl;

        // Data for a variable declaration
        struct {
            Token name;
            struct ASTNode* initializer; // Pointer to the value expression
        } var_decl;
        
        // Data for a return statement
        struct {
            struct ASTNode* value; // Pointer to the returned expression
        } return_stmt;
        
        // Data for a binary expression (e.g., 1 + 2)
        struct {
            struct ASTNode* left;
            Token op;
            struct ASTNode* right;
        } binary_expr;
        
        // Data for a literal expression
        struct {
            Token literal; // The token for the literal (e.g., 42, "hello")
        } literal_expr;

        // Data for an identifier expression (e.g., `x`)
        struct {
            Token identifier;
        } identifier_expr;

        // Data for a block of statements
        struct {
            struct ASTNode** statements;
            int count;
        } block;
    };
} ASTNode;

// Our main parsing function.
ASTNode* quastra_parse_program(Lexer* lexer);

#endif // QUASTR_PARSER_H
