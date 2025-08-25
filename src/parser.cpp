#include "parser.h"
#include "common.h"

// This pointer will hold the lexer state for the parser.
static Lexer* g_lexer;
static Token g_current_token;

// --- Function Prototypes (Forward Declarations) ---
// These declarations tell the compiler about the functions that will be defined later.
static ASTNode* parse_expression();
static ASTNode* parse_statement();
static ASTNode* parse_declaration();
static ASTNode* parse_literal_expression();
static ASTNode* parse_identifier_expression();
static ASTNode* parse_primary_expression();
static ASTNode* parse_binary_expression();
static ASTNode* parse_return_statement();
static ASTNode* parse_var_declaration();
static ASTNode* parse_block_statement();
static ASTNode* parse_fn_declaration();
static void consume_or_error(TokenType type, const char* message);

// Helper function to advance to the next token.
static void advance() {
    g_current_token = quastra_lexer_scan_token(g_lexer);
}

// Consumes the current token if it matches the expected type.
static bool consume(TokenType type) {
    if (g_current_token.type == type) {
        advance();
        return true;
    }
    return false;
}

static void consume_or_error(TokenType type, const char* message) {
    if (!consume(type)) {
        fprintf(stderr, "Parsing Error: %s\n", message);
        exit(1);
    }
}

// Allocates and initializes a new AST node.
static ASTNode* new_node(ASTNodeType type) {
    ASTNode* node = new ASTNode();
    if (node == nullptr) {
        fprintf(stderr, "Error: Out of memory.\n");
        exit(1);
    }
    node->type = type;
    return node;
}

// --- Parsing Functions (Recursive Descent) ---

// Parses a literal expression (e.g., 123, "hello", true).
static ASTNode* parse_literal_expression() {
    ASTNode* node = new_node(NODE_LITERAL_EXPR);
    node->literal_expr.literal = g_current_token;
    advance();
    return node;
}

// Parses an identifier expression (e.g., `x`).
static ASTNode* parse_identifier_expression() {
    ASTNode* node = new_node(NODE_IDENTIFIER_EXPR);
    node->identifier_expr.identifier = g_current_token;
    advance();
    return node;
}

// Parses a term in an expression (e.g., a number, an identifier).
static ASTNode* parse_primary_expression() {
    if (g_current_token.type == TOKEN_INT_LITERAL || g_current_token.type == TOKEN_FLOAT_LITERAL) {
        return parse_literal_expression();
    }
    if (g_current_token.type == TOKEN_IDENTIFIER) {
        return parse_identifier_expression();
    }
    return nullptr; // For now, only handle literals and identifiers.
}

// Parses a simple binary expression like 1 + 2.
static ASTNode* parse_binary_expression() {
    ASTNode* left = parse_primary_expression();
    if (!left) return nullptr;
    
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

// Parses a variable declaration.
static ASTNode* parse_var_declaration() {
    ASTNode* node = new_node(NODE_VAR_DECL);
    advance(); // Consume `let` keyword
    
    // Optional `mut` keyword
    if (g_current_token.type == TOKEN_KEYWORD_MUT) {
        advance();
    }
    
    node->var_decl.name = g_current_token;
    advance(); // Consume identifier name
    
    // Check for optional type annotation
    if (g_current_token.type == TOKEN_COLON) {
        advance(); // Consume `:`
        advance(); // Consume type identifier (we don't handle it yet)
    }

    // Check for initializer
    if (consume(TOKEN_ASSIGN)) {
        node->var_decl.initializer = parse_expression();
    }
    
    consume(TOKEN_SEMICOLON); // Consume optional semicolon
    
    return node;
}

// Parses a block of statements
static ASTNode* parse_block_statement() {
    ASTNode* node = new_node(NODE_BLOCK);
    node->block.statements = new ASTNode*[100]; // Pre-allocate a reasonable size
    node->block.count = 0;

    // Loop until we find the closing brace or end of file
    while (g_current_token.type != TOKEN_RIGHT_BRACE && g_current_token.type != TOKEN_EOF) {
        if (node->block.count >= 100) {
            fprintf(stderr, "Parsing Error: Block statement too large.\n");
            exit(1);
        }
        node->block.statements[node->block.count] = parse_statement();
        node->block.count++;
    }
    return node;
}

// Parses a single statement.
static ASTNode* parse_statement() {
    if (g_current_token.type == TOKEN_KEYWORD_RETURN) {
        return parse_return_statement();
    }
    if (g_current_token.type == TOKEN_KEYWORD_LET) {
        return parse_var_declaration();
    }
    return nullptr;
}

// Parses a single declaration (e.g., a variable or function).
static ASTNode* parse_declaration() {
    if (g_current_token.type == TOKEN_KEYWORD_FN) {
        return parse_fn_declaration();
    }
    return nullptr;
}

// The top-level parsing function for a function declaration.
static ASTNode* parse_fn_declaration() {
    ASTNode* node = new_node(NODE_FN_DECL);
    advance(); // Consume `fn` keyword
    
    if (g_current_token.type != TOKEN_IDENTIFIER) {
        fprintf(stderr, "Parsing Error: Expected function name after 'fn'.\n");
        exit(1);
    }
    node->fn_decl.name = g_current_token;
    advance(); // Consume function name token
    
    consume_or_error(TOKEN_LEFT_PAREN, "Expected '(' after function name.");
    consume_or_error(TOKEN_RIGHT_PAREN, "Expected ')' after function parameters.");
    
    // Handle the `-> type` syntax
    if (g_current_token.type == TOKEN_MINUS_GREATER) {
        advance();
        if (g_current_token.type != TOKEN_IDENTIFIER) {
            fprintf(stderr, "Parsing Error: Expected return type after '->'.\n");
            exit(1);
        }
        advance();
    }

    consume_or_error(TOKEN_LEFT_BRACE, "Expected '{' for function body.");
    node->fn_decl.body = parse_block_statement();
    consume_or_error(TOKEN_RIGHT_BRACE, "Expected '}' after function body.");
    
    return node;
}

// The main entry point for the parser.
static ASTNode* parse_expression() {
    return parse_binary_expression();
}

ASTNode* quastra_parse_program(Lexer* lexer) {
    g_lexer = lexer;
    advance();
    return parse_declaration();
}
