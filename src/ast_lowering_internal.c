#include <stdlib.h>
#include "ast_lowering_internal.h"

#define self_t AstLowering *self

// SECTION: Utilities
// General utilities for adding and manipulating instructions/extra data

static HirIndex add_inst(self_t, HirInstTag tag, HirInstData data) {
    hir_inst_list_add(&self->instructions, (HirInst) {tag, data});
    return self->instructions.size - 1;
}

static HirIndex add_extra(self_t, HirIndex data) {
    index_list_add(&self->extra, data);
    return self->extra.size - 1;
}

static HirIndex reserve_inst(self_t) {
    return add_inst(self, HIR_RESERVED, (HirInstData) {});
}

static HirIndex fill_inst(self_t, HirIndex reserved, HirInstTag tag, HirInstData data) {
    HirInst *inst = hir_inst_list_get(&self->instructions, reserved);
    assert(inst->tag == HIR_RESERVED);
    inst->tag = tag;
    inst->data = data;
    return reserved;
}


// SECTION: Implementation
// The implementation of lowering each ast node.

HirIndex ast_lower_module(self_t, AstIndex module_index) {
    return 0;
}


// Top level declarations

HirIndex ast_lower_const(self_t, AstNode *node) {
    assert(node->tag == AST_CONST);
    HirIndex result = reserve_inst(self);

    // Intern the name of the declaration
    char *name_bytes = ast_get_token_content(self->ast, node->main_token + 1);
    StringKey name = string_set_add(&self->strings, name_bytes);
    free(name_bytes);

    // Parse the initializer
    assert(node->data.rhs != ast_index_empty);
    HirIndex init_expr = ast_lower_expr(self, node->data.rhs);

    // If there is a type annotation, insert an `as_type` instruction
    //todo

    return fill_inst(self, result, HIR_CONST_DECL, (HirInstData) {
        .pl_op = {
            .payload = name,
            .operand = init_expr,
        }
    });
}

HirIndex ast_lower_tl_decl(self_t, AstIndex decl_index) {
    AstNode *node = ast_get_node(self->ast, decl_index);

    HirIndex result;
    switch (node->tag) {
        case AST_CONST: {
            result = ast_lower_const(self, node);
            break;
        }
        default:
            assert(false);
    }

    return result;
}


// Statements

HirIndex ast_lower_stmt(self_t, AstIndex decl_index) {
    assert(false);
}


// Expressions

HirIndex ast_lower_integer(self_t, AstNode *node) {
    assert(node->tag == AST_INTEGER);

    // Parse the integer as a u64
    //todo check for error and if the int is too big, add an HIR_BIG_INT instruction instead
    char *bytes = ast_get_token_content(self->ast, node->main_token);
    uint64_t value = strtoull(bytes, NULL, 10);
    free(bytes);

    return add_inst(self, HIR_INT, (HirInstData) {
        .int_value = value
    });
}

HirIndex ast_lower_expr(self_t, AstIndex decl_index) {
    AstNode *node = ast_get_node(self->ast, decl_index);

    HirIndex result;
    switch (node->tag) {
        case AST_INTEGER: {
            result = ast_lower_integer(self, node);
            break;
        }
        default:
            assert(false);
    }

    return result;
}

#undef self_t
