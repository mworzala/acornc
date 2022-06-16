#include <stdlib.h>
#include <string.h>
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
    // AST const only counts declarations like `const x = 42`, not functions. Even though they both result in HIR_CONST_DECL
    //todo when `const a = fn() {};` is supported by parser, this will not be the case.
    assert(node->tag == AST_CONST);
    HirIndex result = reserve_inst(self);
    HirIndex block_inline = reserve_inst(self);

    // Intern the name of the declaration
    char *name_bytes = ast_get_token_content(self->ast, node->main_token + 1);
    StringKey name = string_set_add(&self->strings, name_bytes);
    free(name_bytes);

    // Parse the initializer
    assert(node->data.rhs != ast_index_empty);
    HirIndex init_expr = ast_lower_expr(self, node->data.rhs);

    // If there is a type annotation, insert an `as_type` instruction
    if (node->data.lhs != ast_index_empty) {
        HirIndex as_type_index = reserve_inst(self);
        HirIndex type_expr = ast_lower_type(self, node->data.lhs);

        init_expr = fill_inst(self, as_type_index, HIR_AS_TYPE, (HirInstData) {
            .pl_op = {
                .payload = type_expr,
                .operand = init_expr,
            }
        });
    }

    fill_inst(self, block_inline, HIR_BLOCK_INLINE, (HirInstData) {
        .un_op = init_expr,
    });

    return fill_inst(self, result, HIR_CONST_DECL, (HirInstData) {
        .pl_op = {
            .payload = name,
            .operand = block_inline,
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
    AstNode *node = ast_get_node(self->ast, decl_index);

    HirIndex result;
    switch (node->tag) {
        //todo let stmt
        default: {
            result = ast_lower_expr(self, decl_index);
            break;
        }
    }

    return result;
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

HirIndex ast_lower_string(self_t, AstNode *node) {
    assert(node->tag == AST_STRING);

    // Intern the string without the quotes
    char *str_bytes = ast_get_token_content(self->ast, node->main_token);
    str_bytes[strlen(str_bytes) - 1] = '\0';
    StringKey str = string_set_add(&self->strings, str_bytes + 1);
    free(str_bytes);

    return add_inst(self, HIR_STRING, (HirInstData) {
        .str_value = str
    });
}

HirIndex ast_lower_bool(self_t, AstNode *node) {
    assert(node->tag == AST_BOOL);

    // Intern the string without the quotes
    char *bytes = ast_get_token_content(self->ast, node->main_token);
    bool value = strcmp(bytes, "true") == 0;
    free(bytes);

    return add_inst(self, HIR_BOOL, (HirInstData) {
        .int_value = value,
    });
}

HirIndex ast_lower_binary(self_t, AstNode *node) {
    assert(node->tag == AST_BINARY);
    HirIndex result = reserve_inst(self);

    // Parse LHS and RHS
    HirIndex lhs = ast_lower_expr(self, node->data.lhs);
    HirIndex rhs = ast_lower_expr(self, node->data.rhs);

    // Determine the instruction based on the operator
    HirInstTag tag;
    char *op_bytes = ast_get_token_content(self->ast, node->main_token);
    if (strcmp(op_bytes, "+") == 0)
        tag = HIR_ADD;
    else if (strcmp(op_bytes, "-") == 0)
        tag = HIR_SUB;
    else if (strcmp(op_bytes, "*") == 0)
        tag = HIR_MUL;
    else if (strcmp(op_bytes, "/") == 0)
        tag = HIR_DIV;
    else assert(false);

    return fill_inst(self, result, tag, (HirInstData) {
        .bin_op = { lhs, rhs }
    });
}

HirIndex ast_lower_block(self_t, AstNode *node) {
    assert(node->tag == AST_BLOCK);

    if (node->data.lhs == ast_index_empty) {
        // Block is empty
        HirIndex extra_index = add_extra(self, 0);
        return add_inst(self, HIR_BLOCK, (HirInstData) {
            .extra = extra_index,
        });
    }

    // Block is not empty, parse statements
    HirIndex result = reserve_inst(self);
    IndexList stmts;
    index_list_init(&stmts);

    for (AstIndex extra_idx = node->data.lhs; extra_idx <= node->data.rhs; extra_idx++) {
        AstIndex stmt_index = self->ast->extra_data.data[extra_idx];
        HirIndex stmt = ast_lower_stmt(self, stmt_index);
        index_list_add(&stmts, stmt);

        //todo do not allow returns in here
        //todo last statement should become an implicit return
        //todo need to add an implicit return field to the AST block node
    }

    // Add the block data to extra_data
    HirBlock block_data = (HirBlock) {.len = stmts.size};
    HirIndex data_index = index_list_add_sized(&self->extra, block_data);
    for (size_t i = 0; i < stmts.size; i++) {
        add_extra(self, stmts.data[i]);
    }

    return fill_inst(self, result, HIR_BLOCK, (HirInstData) {
        .extra = data_index,
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
        case AST_STRING: {
            result = ast_lower_string(self, node);
            break;
        }
        case AST_BOOL: {
            result = ast_lower_bool(self, node);
            break;
        }
        case AST_BINARY: {
            result = ast_lower_binary(self, node);
            break;
        }
        case AST_BLOCK: {
            result = ast_lower_block(self, node);
            break;
        }
        default:
            assert(false);
    }

    return result;
}


// Type

HirIndex ast_lower_type(self_t, AstIndex type_index) {
    AstNode *node = ast_get_node_tagged(self->ast, type_index, AST_TYPE);

    char *bytes = ast_get_token_content(self->ast, node->main_token);
    if (strcmp(bytes, "*") != 0) {
        // Not a pointer
        StringKey type_name = string_set_add(&self->strings, bytes);
        free(bytes);
        return add_inst(self, HIR_TYPE, (HirInstData) {
            .ty = {
                .is_ptr = false,
                .inner = type_name,
            }
        });
    }

    // Type must be a pointer, parse inner
    HirIndex result = reserve_inst(self);

    HirIndex inner_type = ast_lower_type(self, node->data.lhs);

    return fill_inst(self, result, HIR_TYPE, (HirInstData) {
        .ty = {
            .is_ptr = true,
            .inner = inner_type,
        }
    });
}

#undef self_t
