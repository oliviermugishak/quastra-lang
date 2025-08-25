// file: src/codegen.h
#ifndef QUASTR_CODEGEN_H
#define QUASTR_CODEGEN_H

#include "parser.h"
#include <stdio.h>

// The main function for code generation. It traverses the AST
// and writes C++ code directly to the output file.
void quastra_codegen(ASTNode* root, FILE* output);

#endif // QUASTR_CODEGEN_H
