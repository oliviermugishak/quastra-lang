#pragma once

#include "token.hpp"
#include <string>
#include <vector>

namespace Quastra {

class Lexer {
public:
    Lexer(const std::string& source);
    std::vector<Token> scan_tokens();

private:
    void scan_token();
    char advance();
    bool is_at_end();
    char peek();
    
    // Add missing declarations
    bool match(char expected);
    void add_token(TokenType type);

    // Correct return types to void to match implementation
    void identifier();
    void number();

    const std::string source;
    std::vector<Token> tokens;
    size_t start = 0;
    size_t current = 0;
    int line = 1;
};

} // namespace Quastra
