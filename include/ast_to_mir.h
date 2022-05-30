#ifndef CONFIG_AST_TO_MIR_H
#define CONFIG_AST_TO_MIR_H

#include "ast.h"
#include "mir.h"

// SECTION: Scope (AtmScope = ast-to-mir scope)
// A basic map tree from string name to MirIndex

typedef enum atm_scope_item_type_s {
    AtmScopeItemTypeVar,
    AtmScopeItemTypeFn,
    AtmScopeItemTypeArg,
} AtmScopeItemType;

typedef struct atm_scope_s {
    uint32_t size;
    uint32_t capacity;
    char **names;
    MirIndex *data;
    AtmScopeItemType *types;
    struct atm_scope_s *parent;
} AtmScope;

#define self_t AtmScope *self

void atm_scope_init(self_t, AtmScope *parent);
void atm_scope_free(self_t);
void atm_scope_set(self_t, const char *name, MirIndex value, AtmScopeItemType type);
MirIndex *atm_scope_get(self_t, const char *name);
AtmScopeItemType *atm_scope_get_type(self_t, const char *name);

#undef self_t

// SECTION: Ast-to-mir
// Lowering stage from AST to MIR.

typedef struct ast_to_mir_s {
    // Inputs
    Ast *ast;

    // Outputs
    MirInstList instructions;
    IndexList extra;

    // Intermediate state
    AtmScope *scope; // Starts as global scope
} AstToMir;

#define self_t AstToMir *self

void ast_to_mir_init(self_t, Ast *ast);
void ast_to_mir_free(self_t);

Mir lower_ast_fn(self_t, AstIndex fn_index);

MirIndex mir_lower_stmt(self_t, AstIndex stmt_index);
MirIndex mir_lower_let(self_t, AstIndex stmt_index);

MirIndex mir_lower_expr(self_t, AstIndex expr_index);
MirIndex mir_lower_int_const(self_t, AstIndex expr_index);
MirIndex mir_lower_ref(self_t, AstIndex expr_index);
MirIndex mir_lower_bin_op(self_t, AstIndex expr_index);
MirIndex mir_lower_call(self_t, AstIndex expr_index);
// If proto_data is not null, its args will be inserted into the function body
MirIndex mir_lower_block(self_t, AstIndex block_index, AstFnProto *proto_data);
MirIndex mir_lower_return(self_t, AstIndex ret_index);

#undef self_t

#endif //CONFIG_AST_TO_MIR_H
