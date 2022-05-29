#ifndef CONFIG_AST_TO_MIR_H
#define CONFIG_AST_TO_MIR_H

#include "ast.h"
#include "mir.h"

typedef struct ast_to_mir_s {
    // Inputs
    Ast *ast;

    // Outputs
    MirInstList instructions;
    IndexList extra;
} AstToMir;

#define self_t AstToMir *self

void ast_to_mir_init(self_t, Ast *ast);

Mir lower_ast_fn(self_t, AstIndex fn_index);

MirIndex mir_lower_expr(self_t, AstIndex expr_index);
MirIndex mir_lower_int_const(self_t, AstIndex expr_index);
MirIndex mir_lower_block(self_t, AstIndex block_index);
MirIndex mir_lower_return(self_t, AstIndex ret_index);

#undef self_t

#endif //CONFIG_AST_TO_MIR_H
