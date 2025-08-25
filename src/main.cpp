// file: src/main.cpp
#include "lexer.h"
#include "parser.h"
#include "codegen.h"
#include "common.h"

// A helper function to print the AST for debugging.
void print_ast(ASTNode* node, int indent) {
    if (node == nullptr) return;
    
    for (int i = 0; i < indent; i++) std::cout << "  ";

    switch (node->type) {
        case NODE_FN_DECL:
            std::cout << "Function: ";
            std::cout.write(node->fn_decl.name.lexeme, node->fn_decl.name.length);
            std::cout << "\n";
            print_ast(node->fn_decl.body, indent + 1);
            break;
        case NODE_VAR_DECL:
            std::cout << "Variable Declaration: ";
            std::cout.write(node->var_decl.name.lexeme, node->var_decl.name.length);
            std::cout << "\n";
            print_ast(node->var_decl.initializer, indent + 1);
            break;
        case NODE_RETURN_STMT:
            std::cout << "Return Statement\n";
            print_ast(node->return_stmt.value, indent + 1);
            break;
        case NODE_BINARY_EXPR:
            std::cout << "Binary Expression: ";
            std::cout.write(node->binary_expr.op.lexeme, node->binary_expr.op.length);
            std::cout << "\n";
            print_ast(node->binary_expr.left, indent + 1);
            print_ast(node->binary_expr.right, indent + 1);
            break;
        case NODE_LITERAL_EXPR:
            std::cout << "Literal: ";
            std::cout.write(node->literal_expr.literal.lexeme, node->literal_expr.literal.length);
            std::cout << "\n";
            break;
        case NODE_IDENTIFIER_EXPR:
            std::cout << "Identifier: ";
            std::cout.write(node->identifier_expr.identifier.lexeme, node->identifier_expr.identifier.length);
            std::cout << "\n";
            break;
        case NODE_BLOCK:
            std::cout << "Block\n";
            for (int i = 0; i < node->block.count; i++) {
                print_ast(node->block.statements[i], indent + 1);
            }
            break;
        default:
            std::cout << "Unknown Node Type: " << node->type << "\n";
            break;
    }
}

// Function to read the entire file into a string.
std::string read_file(const char* path) {
    std::ifstream file(path, std::ios::in | std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Could not open file '" << path << "'.\n";
        return "";
    }
    std::string source_code((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    return source_code;
}

// Main function for the compiler.
int main(int argc, const char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: quastra <file_path>\n";
        return 64;
    }
    
    std::string source_code = read_file(argv[1]);
    if (source_code.empty()) {
        return 65;
    }

    // Phase 1: Lexing and Parsing
    Lexer lexer;
    quastra_lexer_init(&lexer, source_code.c_str());
    ASTNode* root_node = quastra_parse_program(&lexer);

    // Phase 2: Code Generation
    std::ofstream output_file("out.cpp");
    if (!output_file.is_open()) {
        std::cerr << "Could not create temporary file.\n";
        return 1;
    }
    quastra_codegen(root_node, output_file);
    output_file.close();

    // Phase 3: Compiling the C++ code using g++
    std::string command = "g++ -o out out.cpp";
    std::cout << "Compiling with C++ compiler: " << command << "\n";
    int compile_result = system(command.c_str());

    if (compile_result == 0) {
        std::cout << "Successfully compiled and created executable 'out'.\n";
    } else {
        std::cerr << "C++ compilation failed.\n";
    }
    
    // TODO: Implement proper memory deallocation for the AST.
    return compile_result;
}
