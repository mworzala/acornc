#ifndef ACORNC_AST_DEBUG_H
#define ACORNC_AST_DEBUG_H

#include "common.h"
#include "ast.h"

//todo this is EXTREMELY unsafe.
char *ast_debug_print(Ast *ast);

void ast_debug_print_node(char *buffer, Ast *ast, AstIndex index, int indent);

#endif //ACORNC_AST_DEBUG_H