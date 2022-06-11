#ifndef ACORN_AST_DEBUG_TREE_H
#define ACORN_AST_DEBUG_TREE_H

#include "common.h"
#include "ast.h"

char *ast_debug_tree_print(Ast *ast, AstIndex root, bool print_locs);

#endif //ACORN_AST_DEBUG_TREE_H
