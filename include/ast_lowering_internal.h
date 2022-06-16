#ifndef ACORN_AST_LOWERING_INTERNAL_H
#define ACORN_AST_LOWERING_INTERNAL_H

#include "common.h"
#include "ast.h"
#include "hir.h"
#include "interner.h"

// SECTION: AST lowering (to HIR)
// Lowering stage from AST to HIR.

typedef struct ast_lowering_s {
    // Inputs
    Ast *ast;

    // Outputs
    HirInstList instructions;
    IndexList extra;
    StringSet strings;
    //todo errors

    // Intermediate state
    //todo scope
} AstLowering;

#define self_t AstLowering *self

HirIndex ast_lower_module(self_t, AstIndex module_index);
HirIndex ast_lower_tl_decl(self_t, AstIndex decl_index);
HirIndex ast_lower_stmt(self_t, AstIndex decl_index);
HirIndex ast_lower_expr(self_t, AstIndex decl_index);

#undef self_t

#endif //ACORN_AST_LOWERING_INTERNAL_H
