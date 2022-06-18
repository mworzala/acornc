#include <stdlib.h>
#include "ast_lowering.h"

#include "ast_lowering_internal.h"

Hir ast_lower(Ast *ast) {
    AstLowering lowering;
    ast_lowering_init(&lowering, ast);

    // Lower root (as a module)
    ast_lower_module(&lowering, ast_index_root);

    ast_lowering_free(&lowering);

    // Construct HIR
    return (Hir) {
        .instructions = lowering.instructions,
        .extra = lowering.extra,
        //todo errors
    };
}