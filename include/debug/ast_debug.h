#ifndef ACORNC_AST_DEBUG_H
#define ACORNC_AST_DEBUG_H

#include "common.h"
#include "ast.h"

void ast_debug_print(Ast *ast);

void ast_debug_print_node(Ast *ast, AstIndex index, int indent);

#endif //ACORNC_AST_DEBUG_H
