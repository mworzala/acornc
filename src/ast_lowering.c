#include "ast_lowering.h"

#include "ast_lowering_internal.h"

Hir ast_lower(Ast *ast) {
    AstLowering lowering;

    // Setup
    lowering.ast = ast;

    hir_inst_list_init(&lowering.instructions);
    index_list_init(&lowering.extra);
    string_set_init(&lowering.strings);
    //todo errors

    //todo scope

    // Lower root (as a module)
    ast_lower_module(&lowering, ast_index_root);

    // Free intermediate state
    //todo scope

    // Construct HIR
    return (Hir) {
        .instructions = lowering.instructions,
        .extra = lowering.extra,
        //todo errors
    };
}