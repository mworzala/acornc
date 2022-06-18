#include <stdlib.h>
#include <string.h>
#include "ast_lowering_internal.h"

// SECTION: AST Scope
//

#define self_t AstScope *self

void ast_scope_init(self_t, AstScope *parent) {
    // Basic check to ensure that we dont create a cycle, but would need to check the whole tree upwards in reality.
    assert(self != parent);

    self->size = 0;
    self->capacity = 0;
    self->names = NULL;
    self->data = NULL;
    self->parent = parent;
}

void ast_scope_free(self_t) {
    ARRAY_FREE(StringKey, self->names);
    ARRAY_FREE(MirIndex, self->data);
    ast_scope_init(self, self->parent);
}

void ast_scope_set(self_t, StringKey name, HirIndex value) {
    if (self->capacity < self->size + 1) {
        self->capacity = ARRAY_GROW_CAPCITY(self->capacity);
        self->names = ARRAY_GROW(StringKey, self->names, self->capacity);
        self->data = ARRAY_GROW(HirIndex, self->data, self->capacity);
    }

    // Replace existing
    for (uint32_t i = 0; i < self->size; i++) {
        if (self->names[i] == name) {
            self->data[i] = value;
            return;
        }
    }

    // Add new to end
    self->names[self->size] = name;
    self->data[self->size] = value;
    self->size++;
}

HirIndex *ast_scope_get(self_t, StringKey name) {
    // Check names
    for (uint32_t i = 0; i < self->size; i++) {
        if (self->names[i] == name) {
            return &self->data[i];
        }
    }

    // Check parent
    if (self->parent != NULL) {
        return ast_scope_get(self->parent, name);
    }

    return NULL;
}

#undef self_t

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

static void scope_push(self_t) {
    AstScope *scope = malloc(sizeof(AstScope));
    ast_scope_init(scope, self->scope);
    self->scope = scope;
}

static void scope_pop(self_t) {
    AstScope *scope = self->scope;
    self->scope = scope->parent;
    ast_scope_free(scope);
    free(scope);
}


// SECTION: Forward declarations & init

void ast_lowering_init(self_t, Ast *ast) {
    self->ast = ast;

    hir_inst_list_init(&self->instructions);
    index_list_init(&self->extra);
    string_set_init(&self->strings);
    //todo errors

    self->scope = malloc(sizeof(AstScope));
    ast_scope_init(self->scope, NULL);
    self->fn_ret_ty = UINT32_MAX;
}

void ast_lowering_free(self_t) {
    assert(self->fn_ret_ty == UINT32_MAX);
    assert(self->scope->parent == NULL);
    free(self->scope);
}

HirIndex ast_lower_block(self_t, AstNode *node);


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

    // Add the declaration to the scope
    ast_scope_set(self->scope, name, result);

    return fill_inst(self, result, HIR_CONST_DECL, (HirInstData) {
        .pl_op = {
            .payload = name,
            .operand = block_inline,
        }
    });
}

HirIndex ast_lower_fn_param(self_t, AstNode *node) {
    assert(node->tag == AST_FN_PARAM);
    HirIndex result = reserve_inst(self);

    // Intern the name of the parameter
    char *name_bytes = ast_get_token_content(self->ast, node->main_token);
    StringKey name = string_set_add(&self->strings, name_bytes);
    free(name_bytes);

    // Add to scope
    ast_scope_set(self->scope, name, result);

    // Parse the type annotation
    assert(node->data.rhs != ast_index_empty);
    HirIndex type_expr = ast_lower_type(self, node->data.rhs);

    return fill_inst(self, result, HIR_FN_PARAM, (HirInstData) {
        .pl_op = {
            .payload = name,
            .operand = type_expr,
        }
    });
}

HirIndex ast_lower_fn_named(self_t, AstNode *node) {
    assert(node->tag == AST_NAMED_FN);
    AstNode *proto_node = ast_get_node_tagged(self->ast, node->data.lhs, AST_FN_PROTO);
    AstFnProto *proto = index_list_get_sized(&self->ast->extra_data, AstFnProto, proto_node->data.lhs);
    HirIndex const_decl_index = reserve_inst(self);
    HirIndex fn_decl_index = reserve_inst(self);

    // Intern the name of the declaration
    char *name_bytes = ast_get_token_content(self->ast, node->main_token + 1);
    StringKey name = string_set_add(&self->strings, name_bytes);
    free(name_bytes);

    // Return type
    HirIndex ret_ty = hir_index_empty;
    if (proto_node->data.rhs != ast_index_empty) {
        ret_ty = ast_lower_type(self, proto_node->data.rhs);
    }

    // Add fn to current scope and enter a new scope
    ast_scope_set(self->scope, name, fn_decl_index);
    scope_push(self);

    // Lower parameters
    IndexList param_indices;
    index_list_init(&param_indices);
    if (proto->param_start != ast_index_empty) {
        for (AstIndex param_index = proto->param_start; param_index <= proto->param_end; param_index++) {
            AstNode *param_node = ast_get_node_tagged(self->ast, self->ast->extra_data.data[param_index], AST_FN_PARAM);
            HirIndex param_hir = ast_lower_fn_param(self, param_node);
            index_list_add(&param_indices, param_hir);
        }
    }

    // Lower the body
    HirIndex body = hir_index_empty;
    if (!(proto->flags & FN_PROTO_FOREIGN)) {
        // Set the return type for use in `return` statements
        self->fn_ret_ty = ret_ty;

        AstNode *body_node = ast_get_node_tagged(self->ast, node->data.rhs, AST_BLOCK);
        body = ast_lower_block(self, body_node);

        self->fn_ret_ty = UINT32_MAX;
    }

    // Exit fn scope
    scope_pop(self);

    // Create the function details
    HirFnDecl fn_decl = (HirFnDecl) {
        .flags = proto->flags, // todo these flags may not stay synced with AST.
        .ret_ty = ret_ty,
        .param_len = param_indices.size,
        .body = body,
    };
    HirIndex extra_index = index_list_add_sized(&self->extra, fn_decl);

    // Add the params to extra as required by fn_decl
    for (size_t i = 0; i < param_indices.size; i++)
        add_extra(self, param_indices.data[i]);
    index_list_free(&param_indices);

    fill_inst(self, fn_decl_index, HIR_FN_DECL, (HirInstData) {
        .extra = extra_index,
    });

    return fill_inst(self, const_decl_index, HIR_CONST_DECL, (HirInstData) {
        .pl_op = {
            .payload = name,
            .operand = fn_decl_index,
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
        case AST_NAMED_FN: {
            result = ast_lower_fn_named(self, node);
            break;
        }
        default:
            assert(false);
    }

    return result;
}


// Statements

HirIndex ast_lower_let(self_t, AstNode *node) {
    assert(node->tag == AST_LET);
    HirIndex result = reserve_inst(self);

    // Intern the name of the variable
    char *name_bytes = ast_get_token_content(self->ast, node->main_token + 1);
    StringKey name = string_set_add(&self->strings, name_bytes);
    free(name_bytes);

    // Add to current scope
    ast_scope_set(self->scope, name, result);

    // Type expr
    HirIndex type_expr = hir_index_empty;
    if (node->data.lhs != ast_index_empty) {
        type_expr = ast_lower_type(self, node->data.lhs);
    }

    // Init expr, required for now
    assert(node->data.rhs != ast_index_empty);
    HirIndex init_expr = ast_lower_expr(self, node->data.rhs);

    // Wrap init_expr in as_type if type_expr is present
    if (type_expr != hir_index_empty) {
        init_expr = add_inst(self, HIR_AS_TYPE, (HirInstData) {
            .pl_op = {
                .payload = type_expr,
                .operand = init_expr,
            }
        });
    }

    return fill_inst(self, result, HIR_LET, (HirInstData) {
        .pl_op = {
            //todo name is only here for debugging purposes, it should be stored in a debug instruction or something once i figure out how llvm debug builder works
            .payload = name,
            .operand = init_expr,
        },
    });
}

HirIndex ast_lower_stmt(self_t, AstIndex decl_index) {
    AstNode *node = ast_get_node(self->ast, decl_index);

    HirIndex result;
    switch (node->tag) {
        case AST_LET: {
            result = ast_lower_let(self, node);
            break;
        }
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

    // Get the value of the bool
    char *bytes = ast_get_token_content(self->ast, node->main_token);
    bool value = strcmp(bytes, "true") == 0;
    free(bytes);

    return add_inst(self, HIR_BOOL, (HirInstData) {
        .int_value = value,
    });
}

HirIndex ast_lower_ref(self_t, AstNode *node) {
    assert(node->tag == AST_REF);

    // Intern the string
    char *bytes = ast_get_token_content(self->ast, node->main_token);
    StringKey key = string_set_add(&self->strings, bytes);
    free(bytes);

    // Search for the symbol in scope
    HirIndex *target = ast_scope_get(self->scope, key);
    if (target == NULL) {
        printf("Error: %s is not defined\n", "GET KEY FROM INTERN SET");
        assert(false);
    }

    return add_inst(self, HIR_REF, (HirInstData) {
        .un_op = *target,
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
    else if (strcmp(op_bytes, "==") == 0)
        tag = HIR_CMP_EQ;
    else if (strcmp(op_bytes, "!=") == 0)
        tag = HIR_CMP_NE;
    else if (strcmp(op_bytes, "<") == 0)
        tag = HIR_CMP_LT;
    else if (strcmp(op_bytes, "<=") == 0)
        tag = HIR_CMP_LE;
    else if (strcmp(op_bytes, ">") == 0)
        tag = HIR_CMP_GT;
    else if (strcmp(op_bytes, ">=") == 0)
        tag = HIR_CMP_GE;
    else if (strcmp(op_bytes, "&&") == 0)
        tag = HIR_AND;
    else if (strcmp(op_bytes, "||") == 0)
        tag = HIR_OR;
    else
        assert(false);

    return fill_inst(self, result, tag, (HirInstData) {
        .bin_op = {lhs, rhs}
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

    // Block is not empty

    // Enter a new scope
    scope_push(self);

    // Parse statements
    HirIndex result = reserve_inst(self);
    IndexList stmts;
    index_list_init(&stmts);
    for (AstIndex extra_idx = node->data.lhs; extra_idx <= node->data.rhs; extra_idx++) {
        AstIndex stmt_index = self->ast->extra_data.data[extra_idx];
        HirIndex stmt = ast_lower_stmt(self, stmt_index);
        index_list_add(&stmts, stmt);
    }

    // Exit scope
    scope_pop(self);

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

HirIndex ast_lower_return(self_t, AstNode *node) {
    assert(node->tag == AST_RETURN || node->tag == AST_I_RETURN);
    HirIndex result = reserve_inst(self);

    // Parse the return value if present
    HirIndex ret_value = hir_index_empty;
    if (node->data.lhs != ast_index_empty) {
        ret_value = ast_lower_expr(self, node->data.lhs);
    }

    if (self->fn_ret_ty == UINT32_MAX) {
        // We are not inside a function
        assert(false);
    } else if (self->fn_ret_ty == 0) { // Void return type
        assert(ret_value == hir_index_empty);
    } else {
        // Insert as_type node
        ret_value = add_inst(self, HIR_AS_TYPE, (HirInstData) {
            .pl_op = {
                .payload = self->fn_ret_ty,
                .operand = ret_value,
            }
        });
    }

    return fill_inst(self, result, HIR_RETURN, (HirInstData) {
        .un_op = ret_value,
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
        case AST_REF: {
            result = ast_lower_ref(self, node);
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
        case AST_RETURN:
        case AST_I_RETURN:
            result = ast_lower_return(self, node);
            break;
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
