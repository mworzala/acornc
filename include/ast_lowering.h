#ifndef ACORN_AST_LOWERING_H
#define ACORN_AST_LOWERING_H

#include "common.h"
#include "mir.h"
#include "hir.h"
#include "ast.h"

Hir ast_lower(Ast *ast);

#endif //ACORN_AST_LOWERING_H
