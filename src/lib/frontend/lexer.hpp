#pragma once

#include "token.hpp"
#include <string>
#include <vector>

namespace Quastra {

// The Lexer class is responsible for turning a string of source code
// into a sequence of tokens.
class Lexer {
public:
    // Constructor: takes the source code to be scanned.
    Lexer(const std::string& source);

    // The main method that performs the tokenization.
    std::vector<Token> scan_tokens();

private:
    // Scans a single token from the current position.
    void scan_token();

    // Helper methods for character manipulation and state management.
    char advance();
    bool is_at_end();
    char peek();
    Token make_token(TokenType type);
    Token identifier();
    Token number();

    const std::string source;
    size_t start = 0;
    size_t current = 0;
    int line = 1;
    std::vector<Token> tokens;
};

} // namespace Quastra
