#include <stdlib.h>
#include <string.h>
#include "ast_to_mir.h"

// Section: Scope

#define self_t AtmScope *self

void atm_scope_init(self_t, AtmScope *parent) {
    assert(self != parent); // Basic check to ensure that we dont create a cycle, but needs to check the whole tree upwards in reality.
    self->size = 0;
    self->capacity = 0;
    self->names = NULL;
    self->data = NULL;
    self->parent = parent;
}

void atm_scope_free(self_t) {
    //todo this does free the names array, but it leaves the allocations of each string.
    ARRAY_FREE(char *, self->names);
    ARRAY_FREE(LLVMValueRef, self->data);
    atm_scope_init(self, self->parent);
}

void atm_scope_set(self_t, const char *name, MirIndex value) {
    if (self->capacity < self->size + 1) {
        self->capacity = ARRAY_GROW_CAPCITY(self->capacity);
        self->names = ARRAY_GROW(char *, self->names, self->capacity);
        self->data = ARRAY_GROW(MirIndex, self->data, self->capacity);
    }

    for (uint32_t i = 0; i < self->size; i++) {
        if (strcmp(self->names[i], name) == 0) {
            self->data[i] = value;
            return;
        }
    }

    self->names[self->size] = strdup(name);
    self->data[self->size] = value;
    self->size++;
}

MirIndex *atm_scope_get(self_t, const char *name) {
    for (uint32_t i = 0; i < self->size; i++) {
        if (strcmp(self->names[i], name) == 0) {
            return &self->data[i];
        }
    }

    if (self->parent != NULL) {
        return atm_scope_get(self->parent, name);
    }

    return NULL;
}

#undef self_t


// Section: Ast-to-mir

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

// Returns a string containing the content of the token at the given index.
// The caller owns the string memory.
static char *get_token_content(self_t, TokenIndex token) {
    Token main_token = self->ast->tokens.data[token];
    size_t str_len = main_token.loc.end - main_token.loc.start;
    char *str = malloc(str_len + 1);
    memcpy(str, (const void *) main_token.loc.start, str_len);
    str[str_len] = '\0';
    return str;
}

static void push_scope(self_t) {
    AtmScope *scope = malloc(sizeof(AtmScope));
    atm_scope_init(scope, self->scope);
    self->scope = scope;
}

static void pop_scope(self_t) {
    AtmScope *scope = self->scope;
    self->scope = scope->parent;
    atm_scope_free(scope);
    free(scope);
}

// Public API
void ast_to_mir_init(self_t, Ast *ast) {
    self->ast = ast;

    mir_inst_list_init(&self->instructions);
    index_list_init(&self->extra);

    // Create global scope
    self->scope = malloc(sizeof(AtmScope));
    atm_scope_init(self->scope, NULL);
}

void ast_to_mir_free(self_t) {
    atm_scope_free(self->scope);
    free(self->scope);

    //todo other stuff
}

Mir lower_ast_fn(self_t, AstIndex fn_index) {
    AstNode *node = ast_get_node_tagged(self->ast, fn_index, AST_NAMED_FN);

    // New scope with params
    push_scope(self);
    //todo insert a `param(#)` instruction for each param.

    MirIndex root_index = mir_lower_block(self, node->data.rhs);
    assert(root_index == 0);

    // Cleanup
    pop_scope(self);

    return (Mir) {
        .instructions = self->instructions,
        .extra = self->extra,
    };
}

// Implementation

MirIndex mir_lower_stmt(self_t, AstIndex stmt_index) {
    AstNode *node = ast_get_node(self->ast, stmt_index);
    assert(node != NULL);

    switch (node->tag) {
        case AST_LET:
            return mir_lower_let(self, stmt_index);
        default:
            return mir_lower_expr(self, stmt_index);
    }
}

MirIndex mir_lower_let(self_t, AstIndex stmt_index) {
    AstNode *node = ast_get_node_tagged(self->ast, stmt_index, AST_LET);

    // Type todo
    assert(node->data.lhs == ast_index_empty);

    // Alloc
    MirIndex alloc_index = add_inst(self, MirAlloc, (MirInstData) {});
    // Insert the pointer to the scope
    char *name = get_token_content(self, node->main_token + 1);
    atm_scope_set(self->scope, name, alloc_index);
    free(name);

    // Initializer (must be present for now)
    assert(node->data.rhs != ast_index_empty);
    MirIndex init_index = mir_lower_expr(self, node->data.rhs);

    // Store
    MirIndex store_index = add_inst(self, MirStore, (MirInstData) {
        .bin_op = {
            .lhs = index_to_ref(alloc_index),
            .rhs = index_to_ref(init_index),
        }
    });

    return store_index;
}


MirIndex mir_lower_expr(self_t, AstIndex expr_index) {
    AstNode *node = ast_get_node(self->ast, expr_index);
    assert(node != NULL);

    switch (node->tag) {
        case AST_INTEGER:
            return mir_lower_int_const(self, expr_index);
        case AST_REF:
            return mir_lower_ref(self, expr_index);
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

    // Parse u32
    char *str = get_token_content(self, node->main_token);
    uint32_t value = strtol(str, NULL, 10);
    free(str);

    return add_inst(self, MirConstant, (MirInstData) {
        .ty_pl = {
            .payload = value, //todo this needs to point into values list
        }
    });
}

MirIndex mir_lower_ref(self_t, AstIndex expr_index) {
    AstNode *node = ast_get_node_tagged(self->ast, expr_index, AST_REF);

    // Lookup name in scope
    char *name = get_token_content(self, node->main_token);
    MirIndex *index = atm_scope_get(self->scope, name);

    if (index == NULL) {
        printf("Undefined reference %s!\n", name);
        assert(false);
    }

    free(name);

    // Wrap in a load
    return add_inst(self, MirLoad, (MirInstData) {
        .un_op = index_to_ref(*index)
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

    // Enter block scope
    // There is a slight duplication here in that we enter a new scope for a function and then again for its block.
    push_scope(self);
    MirIndex out_index = reserve_inst(self);

    IndexList insts;
    index_list_init(&insts);

    for (uint32_t index = block->data.lhs; index <= block->data.rhs; index++) {
        AstIndex ast_index = self->ast->extra_data.data[index];
        MirIndex inst_index = mir_lower_stmt(self, ast_index);

        index_list_add(&insts, inst_index);

//        AstNode *node = ast_get_node(self->ast, index);
    }

    MirIndex data_index = add_extra(self, insts.size);
    for (uint32_t i = 0; i < insts.size; i++) {
        add_extra(self, *index_list_get(&insts, i));
    }

    // Cleanup
    pop_scope(self);

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