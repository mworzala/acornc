#ifndef ACORN_AST_LOWERING_INTERNAL_H
#define ACORN_AST_LOWERING_INTERNAL_H

#include "common.h"
#include "ast.h"
#include "hir.h"
#include "interner.h"

// SECTION: AstScope
// A map tree from name key to HirIndex

typedef struct ast_scope_s {
    uint32_t size;
    uint32_t capacity;
    StringKey *names;
    HirIndex *data;
    struct ast_scope_s *parent;
} AstScope;

#define self_t AstScope *self

void ast_scope_init(self_t, AstScope *parent);
void ast_scope_free(self_t);
void ast_scope_set(self_t, StringKey name, HirIndex value);
HirIndex *ast_scope_get(self_t, StringKey name);

#undef self_t

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
    AstScope *scope; // Starts as global scope
    // If 0, the current function return type is void
    // If UINT32_MAX, not inside a function
    // Otherwise, the index contains the return type
    HirIndex fn_ret_ty;
} AstLowering;

#define self_t AstLowering *self

void ast_lowering_init(self_t, Ast *ast);
void ast_lowering_free(self_t);

HirIndex ast_lower_module(self_t, AstIndex module_index);
HirIndex ast_lower_tl_decl(self_t, AstIndex decl_index);
HirIndex ast_lower_stmt(self_t, AstIndex stmt_index);
HirIndex ast_lower_expr(self_t, AstIndex expr_index);
HirIndex ast_lower_type(self_t, AstIndex type_index);

#undef self_t

#endif //ACORN_AST_LOWERING_INTERNAL_H
