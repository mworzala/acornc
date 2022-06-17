#include "debug/ast_debug_tree.h"

#include <stdlib.h>
#include <string.h>

typedef struct {
    // Options
    bool print_locs;

    Ast *ast;

    char *buffer;
    size_t buffer_index;
} AstDebug;

#define AST_DEBUG_BUFFER_SIZE (sizeof(char) * 4096)

#define self_t AstDebug *self

// SECTION: Utilities
// Utility macros and functions for retrieving or emitting information.

#define print(self, ...) { \
    sprintf(self->buffer + self->buffer_index, __VA_ARGS__); \
    self->buffer_index += strlen(self->buffer + self->buffer_index); \
}

#define get_extra(self, index) *index_list_get(&(self)->ast->extra_data, (index))

void print_token(self_t, TokenIndex token_idx) {
    char *str = ast_get_token_content(self->ast, token_idx);
    print(self, "\"%s\"", str);
    free(str);
}


// SECTION: Forward declarations
// Any elements which need forward declarations are done here

static void print_node(self_t, AstIndex index, int indent);

static void print_node_if_present(self_t, AstIndex index, int indent) {
    if (index == ast_index_empty)
        return;
    print_node(self, index, indent);
}


// SECTION: Implementation
// Debug implementation handled here.

static void print_main_token_generic(self_t, AstNode *node) {
    print_token(self, node->main_token);
    print(self, "\n");
}

static void print_nothing_generic(self_t, AstNode *node) {
    // This index subtraction removes the space added by print_node.
    self->buffer_index--;
    print(self, "\n");

}

static void print_dot(self_t, AstNode *node, int indent) {
    // This index subtraction removes the space added by print_node.
    self->buffer_index--;
    print(self, "\n");

    // Print LHS/RHS
    print_node(self, node->data.lhs, indent + 2);
    print_node(self, node->data.rhs, indent + 2);
}

static void print_binary(self_t, AstNode *node, int indent) {
    print_token(self, node->main_token);
    print(self, "\n");

    // Print LHS/RHS
    print_node(self, node->data.lhs, indent + 2);
    print_node(self, node->data.rhs, indent + 2);
}

static void print_return(self_t, AstNode *node, int indent) {
    // This index subtraction removes the space added by print_node.
    self->buffer_index--;
    print(self, "\n");

    // Print LHS
    print_node_if_present(self, node->data.lhs, indent + 2);
}

static void print_i_return(self_t, AstNode *node, int indent) {
    // This index subtraction removes the space added by print_node.
    self->buffer_index--;
    print(self, "\n");

    // Print LHS
    print_node_if_present(self, node->data.lhs, indent + 2);
}

static void print_block(self_t, AstNode *node, int indent) {
    // This index subtraction removes the space added by print_node.
    self->buffer_index--;
    print(self, "\n");

    if (node->data.lhs == ast_index_empty)
        return;

    for (uint32_t i = node->data.lhs; i <= node->data.rhs; i++) {
        print_node(self, get_extra(self, i), indent + 2);
    }
}

static void print_call(self_t, AstNode *node, int indent) {
    AstCallData call_data = *((AstCallData*) &get_extra(self, node->data.rhs));
    // This index subtraction removes the space added by print_node.
    self->buffer_index--;
    print(self, "\n");

    // Operand
    print_node(self, node->data.lhs, indent + 2);

    // Arguments
    if (call_data.arg_start == ast_index_empty)
        return;

    for (uint32_t arg = call_data.arg_start; arg <= call_data.arg_end; arg++) {
        print_node(self, get_extra(self, arg), indent + 2);
    }
}

static void print_if(self_t, AstNode *node, int indent) {
    AstIfData if_data = *((AstIfData*) &get_extra(self, node->data.rhs));
    // This index subtraction removes the space added by print_node.
    self->buffer_index--;
    print(self, "\n");

    // Condition
    print_node(self, node->data.lhs, indent + 2);

    // Then block
    print_node(self, if_data.then_block, indent + 2);

    // Else block
    print_node_if_present(self, if_data.else_block, indent + 2);
}

static void print_while(self_t, AstNode *node, int indent) {
    // This index subtraction removes the space added by print_node.
    self->buffer_index--;
    print(self, "\n");

    // Condition
    print_node(self, node->data.lhs, indent + 2);

    // block
    print_node(self, node->data.rhs, indent + 2);
}

static void print_let(self_t, AstNode *node, int indent) {
    print_token(self, node->main_token + 1);
    print(self, "\n");

    print_node_if_present(self, node->data.lhs, indent + 2);
    print_node_if_present(self, node->data.rhs, indent + 2);
}

static void print_type_expr(self_t, AstNode *node, int indent) {
    print_token(self, node->main_token);
    print(self, "\n");

    print_node_if_present(self, node->data.lhs, indent + 2);
}

static void print_const(self_t, AstNode *node, int indent) {
    print_token(self, node->main_token + 1);
    print(self, "\n");

    print_node_if_present(self, node->data.lhs, indent + 2);
    print_node_if_present(self, node->data.rhs, indent + 2);
}

static void print_named_fn(self_t, AstNode *node, int indent) {
    print_token(self, node->main_token + 1);
    print(self, "\n");

    print_node(self, node->data.lhs, indent + 2);
    print_node_if_present(self, node->data.rhs, indent + 2);
}

static void print_fn_proto(self_t, AstNode *node, int indent) {
    // This index subtraction removes the space added by print_node.
    self->buffer_index--;
    print(self, "\n");

    AstFnProto proto = *((AstFnProto*) &get_extra(self, node->data.lhs));

    // Flags
    if (proto.flags != FN_PROTO_NONE) {
        print(self, "%*sflags", indent + 2, "");

        if (proto.flags & FN_PROTO_FOREIGN)
            print(self, " foreign");

        print(self, "\n");
    }

    // Print parameters
    if (proto.param_start != ast_index_empty) {
        for (uint32_t i = proto.param_start; i <= proto.param_end; i++) {
            print_node(self, get_extra(self, i), indent + 2);
        }
    }

    // Return type
    print_node_if_present(self, node->data.rhs, indent + 2);
}

static void print_fn_param(self_t, AstNode *node, int indent) {
    print_token(self, node->main_token);
    print(self, "\n");

    print_node_if_present(self, node->data.rhs, indent + 2);
}

static void print_container_generic(self_t, AstNode *node, int indent) {
    print_token(self, node->main_token + 1);
    print(self, "\n");

    if (node->data.lhs == ast_index_empty)
        return;

    for (uint32_t i = node->data.lhs; i <= node->data.rhs; i++) {
        print_node(self, get_extra(self, i), indent + 2);
    }
}

static void print_module(self_t, AstNode *node, int indent) {
    // This index subtraction removes the space added by print_node.
    self->buffer_index--;
    print(self, "\n");

    if (node->data.lhs == ast_index_empty)
        return;

    for (uint32_t i = node->data.lhs; i <= node->data.rhs; i++) {
        print_node(self, get_extra(self, i), indent + 2);
    }
}


static void print_node(self_t, AstIndex index, int indent) {
    AstNode *node = ast_get_node(self->ast, index);
    assert(node != NULL);

    // Print the indentation & node name
    print(self, "%*s%s", indent, "", ast_tag_to_string(node->tag));

    if (self->print_locs) {
        //todo cannot really implement this atm. AST does not have locational information.
//        Token main_token = self->ast->tokens.data
//        print(self, "@%d..%d", 0, 1);
    }

    print(self, " ");

    switch (node->tag) {
        // Expressions
        case AST_INTEGER:
        case AST_STRING:
        case AST_BOOL:
        case AST_REF:
            print_main_token_generic(self, node);
            break;
        case AST_DOT:
            print_dot(self, node, indent);
            break;
        case AST_BINARY:
            print_binary(self, node, indent);
            break;
        case AST_RETURN:
            print_return(self, node, indent);
            break;
        case AST_I_RETURN:
            print_i_return(self, node, indent);
            break;
        case AST_BLOCK:
            print_block(self, node, indent);
            break;
        case AST_CALL:
            print_call(self, node, indent);
            break;
        case AST_IF:
            print_if(self, node, indent);
            break;
        case AST_WHILE:
            print_while(self, node, indent);
            break;

        // Statements
        case AST_LET:
            print_let(self, node, indent);
            break;

        // Declarations
        case AST_CONST:
            print_const(self, node, indent);
            break;
        case AST_NAMED_FN:
            print_named_fn(self, node, indent);
            break;
        case AST_FN_PROTO:
            print_fn_proto(self, node, indent);
            break;
        case AST_FN_PARAM:
            print_fn_param(self, node, indent);
            break;
        case AST_STRUCT:
        case AST_ENUM:
            print_container_generic(self, node, indent);
            break;
        case AST_FIELD:
            print_fn_param(self, node, indent);
            break;
        case AST_ENUM_CASE:
            print_main_token_generic(self, node);
            break;

        // Other
        case AST_MODULE:
            print_module(self, node, indent);
            break;
        case AST_TYPE:
            print_type_expr(self, node, indent);
            break;
        case AST_ERROR:
            print_nothing_generic(self, node);
            break;
        default:
            assert(false);
    }
}

#undef self_t


// SECTION: Public API

char *ast_debug_tree_print(Ast *ast, AstIndex root, bool print_locs) {
    AstDebug self = {
        .print_locs = print_locs,
        .ast = ast,
        .buffer = malloc(AST_DEBUG_BUFFER_SIZE),
        .buffer_index = 0,
    };

    print_node(&self, root, 0);

//    ast_debug_print_node_raw(&self, root, 0);

    return self.buffer;
}
