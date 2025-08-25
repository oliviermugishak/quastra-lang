#include "parser.h"
#include "common.h"

// This pointer will hold the lexer state for the parser.
static Lexer* g_lexer;
static Token g_current_token;

// Helper function to advance to the next token.
static void advance() {
    g_current_token = quastra_lexer_scan_token(g_lexer);
}

// Consumes the current token if it matches the expected type.
static int consume(TokenType type) {
    if (g_current_token.type == type) {
        advance();
        return 1;
    }
    return 0;
}

// Allocates and initializes a new AST node.
static ASTNode* new_node(ASTNodeType type) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    if (node == NULL) {
        fprintf(stderr, "Error: Out of memory.\n");
        exit(1);
    }
    node->type = type;
    return node;
}

// --- Parsing Functions (Recursive Descent) ---

static ASTNode* parse_expression();
static ASTNode* parse_statement();

// Parses a literal expression (e.g., 123, "hello", true).
static ASTNode* parse_literal_expression() {
    ASTNode* node = new_node(NODE_LITERAL_EXPR);
    node->literal_expr.literal = g_current_token;
    advance();
    return node;
}

// Parses a term in an expression (e.g., a number).
static ASTNode* parse_primary_expression() {
    if (g_current_token.type == TOKEN_INT_LITERAL || g_current_token.type == TOKEN_FLOAT_LITERAL) {
        return parse_literal_expression();
    }
    return NULL; // For now, only handle literals.
}

// Parses a simple binary expression like 1 + 2.
static ASTNode* parse_binary_expression() {
    ASTNode* left = parse_primary_expression();
    if (!left) return NULL;
    
    // Check for an operator
    while (g_current_token.type == TOKEN_PLUS || g_current_token.type == TOKEN_MINUS) {
        ASTNode* node = new_node(NODE_BINARY_EXPR);
        node->binary_expr.left = left;
        node->binary_expr.op = g_current_token;
        advance();
        node->binary_expr.right = parse_primary_expression();
        left = node;
    }
    
    return left;
}

// Parses a return statement.
static ASTNode* parse_return_statement() {
    ASTNode* node = new_node(NODE_RETURN_STMT);
    advance(); // Consume `return` keyword
    node->return_stmt.value = parse_expression();
    consume(TOKEN_SEMICOLON); // Consume optional semicolon
    return node;
}

// Main function for parsing an expression.
static ASTNode* parse_expression() {
    return parse_binary_expression();
}

// Parses a single statement.
static ASTNode* parse_statement() {
    if (g_current_token.type == TOKEN_KEYWORD_RETURN) {
        return parse_return_statement();
    }
    // TODO: Add support for other statements.
    return NULL;
}

// The top-level parsing function for a function declaration.
static ASTNode* parse_fn_declaration() {
    ASTNode* node = new_node(NODE_FN_DECL);
    advance(); // Consume `fn` keyword
    
    node->fn_decl.name = g_current_token;
    advance(); // Consume function name token
    
    // For now, assume a simple `fn main() -> type { ... }` structure.
    consume(TOKEN_LEFT_PAREN);
    consume(TOKEN_RIGHT_PAREN);
    consume(TOKEN_SEMICOLON); // optional
    
    // TODO: Handle return type
    while (g_current_token.type != TOKEN_LEFT_BRACE) {
        advance();
    }
    
    // Parse the function body
    consume(TOKEN_LEFT_BRACE);
    node->fn_decl.body = parse_statement();
    consume(TOKEN_RIGHT_BRACE);
    
    return node;
}

// The main entry point for the parser.
ASTNode* quastra_parse_program(Lexer* lexer) {
    g_lexer = lexer;
    advance(); // Prime the lexer by getting the first token.
    return parse_fn_declaration();
}
