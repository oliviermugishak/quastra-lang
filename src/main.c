#include "lexer.h"
#include "parser.h"
#include "common.h"
#include "banner.h"

// A helper function to print the AST for debugging.
void print_ast(ASTNode* node, int indent) {
    if (node == NULL) return;
    
    // Indentation for tree-like output.
    for (int i = 0; i < indent; i++) printf("  ");

    switch (node->type) {
        case NODE_FN_DECL:
            printf("Function: %.*s\n", node->fn_decl.name.length, node->fn_decl.name.lexeme);
            print_ast(node->fn_decl.body, indent + 1);
            break;
        case NODE_RETURN_STMT:
            printf("Return Statement\n");
            print_ast(node->return_stmt.value, indent + 1);
            break;
        case NODE_BINARY_EXPR:
            printf("Binary Expression: %.*s\n", node->binary_expr.op.length, node->binary_expr.op.lexeme);
            print_ast(node->binary_expr.left, indent + 1);
            print_ast(node->binary_expr.right, indent + 1);
            break;
        case NODE_LITERAL_EXPR:
            printf("Literal: %.*s\n", node->literal_expr.literal.length, node->literal_expr.literal.lexeme);
            break;
        default:
            printf("Unknown Node Type: %d\n", node->type);
            break;
    }
}

// Function to read the entire file into a string.
char* read_file(const char* path) {
    FILE* file = fopen(path, "rb");
    if (file == NULL) {
        fprintf(stderr, "Could not open file '%s'.\n", path);
        return NULL;
    }

    fseek(file, 0L, SEEK_END);
    size_t fileSize = ftell(file);
    rewind(file);

    char* buffer = (char*)malloc(fileSize + 1);
    if (buffer == NULL) {
        fprintf(stderr, "Not enough memory to read file '%s'.\n", path);
        fclose(file);
        return NULL;
    }

    size_t bytesRead = fread(buffer, sizeof(char), fileSize, file);
    if (bytesRead < fileSize) {
        fprintf(stderr, "Could not read file '%s'.\n", path);
        free(buffer);
        fclose(file);
        return NULL;
    }

    buffer[bytesRead] = '\0';
    fclose(file);
    return buffer;
}

// Main function for the compiler.
int main(int argc, const char* argv[]) {

    // Call the function to print the banner
    printBanner();

    if (argc != 2) {
        fprintf(stderr, "Usage: zen_compiler <file_path>\n");
        exit(64);
    }
    
    char* source = read_file(argv[1]);
    if (source == NULL) {
        exit(65);
    }

    Lexer lexer;
    zen_lexer_init(&lexer, source);

    ASTNode* root_node = zen_parse_program(&lexer);
    
    printf("--- AST --- \n");
    print_ast(root_node, 0);

    // Free resources.
    // TODO: Implement a proper memory deallocator for the AST.
    free(source);
    
    return 0;
}
