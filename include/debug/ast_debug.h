#ifndef ACORNC_AST_DEBUG_H
#define ACORNC_AST_DEBUG_H

#include "common.h"
#include "ast.h"


typedef void (*AstDebugFn)(char *, Ast *, AstIndex, AstNode *, Token *, int);

extern AstDebugFn ast_debug_fns[__AST_LAST];

//todo this is EXTREMELY unsafe.
char *ast_debug_print(Ast *ast, AstIndex root);

void ast_debug_print_node_raw(char *buffer, Ast *ast, AstIndex index, int indent);
void ast_debug_print_node(char *buffer, Ast *ast, AstIndex index, int indent);

#endif //ACORNC_AST_DEBUG_H
