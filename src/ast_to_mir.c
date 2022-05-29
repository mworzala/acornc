#include "ast_to_mir.h"

#define self_t AstToMir *self


// Utilities
static void add_inst(self_t, MirInstTag tag, MirInstData data) {
    mir_inst_list_add(&self->instructions, (MirInst) {tag, data});
}


// Public API
void ast_to_mir_init(self_t, Ast *ast) {
    self->ast = ast;
}

Mir lower_ast_fn(self_t, AstIndex fn_index) {
    AstNode *node = ast_get_node_tagged(self->ast, fn_index, AST_NAMED_FN);
}


// Implementation

#undef self_t