#include <stdlib.h>
#include <string.h>
#include "ast_to_mir.h"

#define self_t AstToMir *self


// Utilities
static inline MirIndex add_inst(self_t, MirInstTag tag, MirInstData data) {
    mir_inst_list_add(&self->instructions, (MirInst) {tag, data});
    return self->instructions.size - 1;
}

static inline MirIndex add_extra(self_t, MirIndex data) {
    index_list_add(&self->extra, data);
    return self->extra.size - 1;
}

static MirIndex reserve_inst(self_t) {
    return add_inst(self, MirReserved, (MirInstData) {});
}

static MirIndex fill_inst(self_t, MirIndex reserved, MirInstTag tag, MirInstData data) {
    MirInst *inst = mir_inst_list_get(&self->instructions, reserved);
    assert(inst->tag == MirReserved);
    inst->tag = tag;
    inst->data = data;
    return reserved;
}

// Public API
void ast_to_mir_init(self_t, Ast *ast) {
    self->ast = ast;

    mir_inst_list_init(&self->instructions);
    index_list_init(&self->extra);
}

Mir lower_ast_fn(self_t, AstIndex fn_index) {
    AstNode *node = ast_get_node_tagged(self->ast, fn_index, AST_NAMED_FN);

    MirIndex root_index = mir_lower_block(self, node->data.rhs);
    assert(root_index == 0);

    return (Mir) {
        .instructions = self->instructions,
        .extra = self->extra,
    };
}

// Implementation


MirIndex mir_lower_expr(self_t, AstIndex expr_index) {
    AstNode *node = ast_get_node(self->ast, expr_index);
    assert(node != NULL);

    switch (node->tag) {
        case AST_INTEGER:
            return mir_lower_int_const(self, expr_index);
        case AST_BLOCK:
            return mir_lower_block(self, expr_index);
        case AST_RETURN:
            return mir_lower_return(self, expr_index);
        default: {
            printf("Cannot lower %s as expr!\n", ast_tag_to_string(node->tag));
            assert(false);
        }
    }
}

MirIndex mir_lower_int_const(self_t, AstIndex expr_index) {
    AstNode *node = ast_get_node_tagged(self->ast, expr_index, AST_INTEGER);

    Token main_token = self->ast->tokens.data[node->main_token];
    size_t str_len = main_token.loc.end - main_token.loc.start;
    char *str = malloc(str_len + 1);
    memcpy(str, (const void *) main_token.loc.start, str_len);
    str[str_len] = '\0';

    // Parse u32
    uint32_t value = strtol(str, NULL, 10);
    free(str);

    return add_inst(self, MirConstant, (MirInstData) {
        .ty_pl = {
            .payload = value, //todo this needs to point into values list
        }
    });
}

MirIndex mir_lower_block(self_t, AstIndex block_index) {
    AstNode *block = ast_get_node_tagged(self->ast, block_index, AST_BLOCK);

    if (block->data.lhs == ast_index_empty) {
        MirIndex extra_index = add_extra(self, 0);
        return add_inst(self, MirBlock, (MirInstData) {
            .ty_pl = {.payload = extra_index},
        });
    }

    MirIndex out_index = reserve_inst(self);

    IndexList insts;
    index_list_init(&insts);

    for (uint32_t index = block->data.lhs; index <= block->data.rhs; index++) {
        MirIndex inst_index = mir_lower_expr(self, index);

        index_list_add(&insts, inst_index);

//        AstNode *node = ast_get_node(self->ast, index);
    }

    MirIndex data_index = add_extra(self, insts.size);
    for (uint32_t i = 0; i < insts.size; i++) {
        add_extra(self, *index_list_get(&insts, i));
    }

    return fill_inst(self, out_index, MirBlock, (MirInstData) {
        .ty_pl = {
            .payload = data_index,
        }
    });
}

MirIndex mir_lower_return(self_t, AstIndex ret_index) {
    AstNode *ret = ast_get_node_tagged(self->ast, ret_index, AST_RETURN);

    if (ret->data.lhs == ast_index_empty) {
        return add_inst(self, MirRet, (MirInstData) {
            .un_op = RefZero,
        });
    }

    MirIndex expr_index = mir_lower_expr(self, ret->data.lhs);

    return add_inst(self, MirRet, (MirInstData) {
        .un_op = index_to_ref(expr_index),
    });
}

#undef self_t