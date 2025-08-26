#include "lib/frontend/lexer.hpp"
#include "lib/frontend/parser.hpp"
#include "lib/backend/codegen.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <memory>

// Function to read a source file into a string.
static std::string read_file(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file '" << path << "'." << std::endl;
        exit(74); // IO error
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

// The main compiler pipeline.
static void run(const std::string& source) {
    Quastra::Lexer lexer(source);
    auto tokens = lexer.scan_tokens();
    Quastra::Parser parser(tokens);
    auto statements = parser.parse();

    // Check for parsing errors.
    // A real compiler would have better error reporting.
    for (const auto& stmt : statements) {
        if (!stmt) {
            std::cerr << "Error: Parsing failed." << std::endl;
            return;
        }
    }

    Quastra::CodeGen codegen;
    std::string cpp_source = codegen.generate(statements);

    // For now, we'll just print the generated C++ to the console.
    // The next step would be to save this to a file and invoke g++.
    std::cout << cpp_source;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: quastra-compiler <file.q>" << std::endl;
        return 64; // Command line usage error
    }

    std::string arg = argv[1];

    if (arg == "--version") {
        std::cout << "Quastra Compiler v1.0.0\n"
          << "Copyright (c) 2025 Quastra Systems\n"
          << "Licensed under the MIT License\n"
          << "This compiler translates Quastra source files (.q) into C++ code.\n"
          << "For documentation and updates, visit: https://quastra.dev\n";

        return 0;
    }

    std::string source_path = argv[1];
    std::string source_code = read_file(source_path);
    run(source_code);

    return 0;
}
