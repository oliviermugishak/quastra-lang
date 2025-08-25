#ifndef QUASTR_CODEGEN_H
#define QUASTR_CODEGEN_H

#include "parser.h"
#include <ostream>

// The main function for code generation. It traverses the AST
// and writes C++ code directly to the output file.
void quastra_codegen(ASTNode* root, std::ostream& output);

#endif // QUASTR_CODEGEN_H
