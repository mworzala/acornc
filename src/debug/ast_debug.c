#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include "debug/ast_debug.h"

void ast_debug_append_default_header(char *buffer, AstIndex index, int indent) {
    sprintf(buffer + strlen(buffer), "%*s", indent, "");
    sprintf(buffer + strlen(buffer), "%%%d = ", index);
}

void ast_debug_append_token_content(char *buffer, Ast *ast, TokenIndex token_idx) {
    Token token = ast->tokens.data[token_idx];
    sprintf(buffer + strlen(buffer), "%.*s",
            (int)(token.loc.end - token.loc.start),
            (char *)token.loc.start);
}

//region Expressions

void ast_debug_literal_generic(char *buffer, Ast *ast, AstIndex index, AstNode *node, Token *main_token, int indent) {
    ast_debug_append_default_header(buffer, index, indent);
    sprintf(buffer + strlen(buffer), "%s", ast_tag_to_string(node->tag));
    sprintf(buffer + strlen(buffer), "(");
    ast_debug_append_token_content(buffer, ast, node->main_token);
    sprintf(buffer + strlen(buffer), ")");
}

const char *binary_op_token_to_string(TokenType token) {
    switch (token) {
        case TOK_PLUS:
            return "add";
        case TOK_MINUS:
            return "sub";
        case TOK_STAR:
            return "mul";
        case TOK_SLASH:
            return "div";
        case TOK_EQ:
            return "assign";
        case TOK_EQEQ:
            return "cmp_eq";
        case TOK_BANGEQ:
            return "cmp_ne";
        case TOK_LT:
            return "cmp_lt";
        case TOK_LTEQ:
            return "cmp_le";
        case TOK_GT:
            return "cmp_gt";
        case TOK_GTEQ:
            return "cmp_ge";
        case TOK_AMPAMP:
            return "log_and";
        case TOK_BARBAR:
            return "log_or";

        // This is a special case since it is actually a distinct ast node.
        // The rendering is identical so the print fn for binary op is reused.
        case TOK_DOT:
            return "dot";

        default:
            return "<?>";
    }
}

void ast_debug_binary(char *buffer, Ast *ast, AstIndex index, AstNode *node, Token *main_token, int indent) {
    ast_debug_print_node(buffer, ast, node->data.lhs, indent);
    ast_debug_print_node(buffer, ast, node->data.rhs, indent);

    ast_debug_append_default_header(buffer, index, indent);
    sprintf(buffer + strlen(buffer), "%s(%%%d, %%%d)",
            binary_op_token_to_string(main_token->type),
            node->data.lhs, node->data.rhs);
}

const char *unary_op_token_to_string(TokenType token) {
    switch (token) {
        case TOK_MINUS:
            return "neg";
        case TOK_BANG:
            return "not";
        default:
            return "<?>";
    }
}

void ast_debug_unary(char *buffer, Ast *ast, AstIndex index, AstNode *node, Token *main_token, int indent) {
    ast_debug_print_node(buffer, ast, node->data.lhs, indent);

    ast_debug_append_default_header(buffer, index, indent);
    sprintf(buffer + strlen(buffer), "%s(%%%d)",
            unary_op_token_to_string(main_token->type),
            node->data.lhs);
}

void ast_debug_block(char *buffer, Ast *ast, AstIndex index, AstNode *node, Token *main_token, int indent) {
    ast_debug_append_default_header(buffer, index, indent);

    sprintf(buffer + strlen(buffer), "block(stmts = ");

    if (node->data.lhs == ast_index_empty) {
        sprintf(buffer + strlen(buffer), "_");
    } else {
        AstIndex start = node->data.lhs;
        uint32_t expr_count = node->data.rhs - start + 1; // Add one since end actually references the last node.

        sprintf(buffer + strlen(buffer), "{\n");
        if (expr_count == 1) {
            ast_debug_print_node(buffer, ast, ast->extra_data.data[start], indent + 2);
        } else {
            for (uint32_t i = 0; i < expr_count; i++) {
                sprintf(buffer + strlen(buffer), "%*s// @%d\n", indent + 2, "", i + 1);
                ast_debug_print_node(buffer, ast, ast->extra_data.data[start + i], indent + 2);
            }
        }
        sprintf(buffer + strlen(buffer), "%*s}", indent, "");
    }

    sprintf(buffer + strlen(buffer), ")");
}

void ast_debug_return(char *buffer, Ast *ast, AstIndex index, AstNode *node, Token *main_token, int indent) {
    if (node->data.lhs != ast_index_empty) {
        ast_debug_print_node(buffer, ast, node->data.lhs, indent);
    }

    ast_debug_append_default_header(buffer, index, indent);

    sprintf(buffer + strlen(buffer), "ret");
    if (node->data.lhs != ast_index_empty) {
        sprintf(buffer + strlen(buffer), "(%%%d)", node->data.lhs);
    }
}

void ast_debug_if(char *buffer, Ast *ast, AstIndex index, AstNode *node, Token *main_token, int indent) {
    ast_debug_print_node(buffer, ast, node->data.lhs, indent);
    AstIfData data = *((AstIfData*) &ast->extra_data.data[node->data.rhs]);
    ast_debug_print_node(buffer, ast, data.then_block, indent);
    if (data.else_block != ast_index_empty) {
        ast_debug_print_node(buffer, ast, data.else_block, indent);
    }

    ast_debug_append_default_header(buffer, index, indent);

    sprintf(buffer + strlen(buffer), "if(%%%d, then = %%%d, else = ", node->data.lhs, data.then_block);
    if (data.else_block == ast_index_empty) {
        sprintf(buffer + strlen(buffer), "_");
    } else {
        sprintf(buffer + strlen(buffer), "%%%d", data.else_block);
    }
    sprintf(buffer + strlen(buffer), ")");
}

void ast_debug_while(char *buffer, Ast *ast, AstIndex index, AstNode *node, Token *main_token, int indent) {
    ast_debug_print_node(buffer, ast, node->data.lhs, indent);
    ast_debug_print_node(buffer, ast, node->data.rhs, indent);

    ast_debug_append_default_header(buffer, index, indent);
    sprintf(buffer + strlen(buffer), "while(%%%d, body = %%%d)", node->data.lhs, node->data.rhs);
}

void ast_debug_call(char *buffer, Ast *ast, AstIndex index, AstNode *node, Token *main_token, int indent) {
    ast_debug_append_default_header(buffer, index, indent);
}

//endregion

//region Statements

void ast_debug_stmt_let(char *buffer, Ast *ast, AstIndex index, AstNode *node, Token *main_token, int indent) {
    if (node->data.rhs != ast_index_empty) {
        ast_debug_print_node(buffer, ast, node->data.rhs, indent);
    }

    ast_debug_append_default_header(buffer, index, indent);

    sprintf(buffer + strlen(buffer), "let(");
    ast_debug_append_token_content(buffer, ast, node->main_token + 1);

    sprintf(buffer + strlen(buffer), ", type = ");
    if (node->data.lhs == ast_index_empty) {
        sprintf(buffer + strlen(buffer), "_");
    } else {
        //todo
        assert(false);
    }

    sprintf(buffer + strlen(buffer), ", init = ");
    if (node->data.rhs == ast_index_empty) {
        sprintf(buffer + strlen(buffer), "_");
    } else {
        sprintf(buffer + strlen(buffer), "%%%d", node->data.rhs);
    }

    sprintf(buffer + strlen(buffer), ")");
}

//endregion

//region Top level decls

void ast_debug_fn_named(char *buffer, Ast *ast, AstIndex index, AstNode *node, Token *main_token, int indent) {
    ast_debug_append_default_header(buffer, index, indent);

    sprintf(buffer + strlen(buffer), "fn(");
    ast_debug_append_token_content(buffer, ast, node->main_token + 1);
    sprintf(buffer + strlen(buffer), ", proto = ");

    ast_debug_print_node_raw(buffer, ast, node->data.lhs, 0);

    sprintf(buffer + strlen(buffer), ", body = ");
    if (node->data.rhs == ast_index_empty) {
        sprintf(buffer + strlen(buffer), "_");
    } else {
        sprintf(buffer + strlen(buffer), "{\n");
        ast_debug_print_node(buffer, ast, node->data.rhs, indent + 2);
        sprintf(buffer + strlen(buffer), "%*s}", indent, "");
    }
}

void ast_debug_struct(char *buffer, Ast *ast, AstIndex index, AstNode *node, Token *main_token, int indent) {
    ast_debug_append_default_header(buffer, index, indent);

    sprintf(buffer + strlen(buffer), "struct(");
    ast_debug_append_token_content(buffer, ast, node->main_token + 1);
    sprintf(buffer + strlen(buffer), ", fields = ");

    if (node->data.lhs == ast_index_empty) {
        sprintf(buffer + strlen(buffer), "_");
    } else {
        size_t field_count = node->data.rhs - node->data.lhs + 1;
        sprintf(buffer + strlen(buffer), "[\n");
        for (uint32_t i = 0; i < field_count; i++) {
            ast_debug_print_node(buffer, ast, ast->extra_data.data[node->data.lhs + i], indent + 2);
        }
        sprintf(buffer + strlen(buffer), "%*s]", indent, "");
    }

    sprintf(buffer + strlen(buffer), ")");
}

void ast_debug_enum(char *buffer, Ast *ast, AstIndex index, AstNode *node, Token *main_token, int indent) {
    ast_debug_append_default_header(buffer, index, indent);

    sprintf(buffer + strlen(buffer), "enum(");
    ast_debug_append_token_content(buffer, ast, node->main_token + 1);
    sprintf(buffer + strlen(buffer), ", cases = ");

    if (node->data.lhs == ast_index_empty) {
        sprintf(buffer + strlen(buffer), "_");
    } else {
        size_t case_count = node->data.rhs - node->data.lhs + 1;
        sprintf(buffer + strlen(buffer), "[\n");
        for (uint32_t i = 0; i < case_count; i++) {
            ast_debug_print_node(buffer, ast, ast->extra_data.data[node->data.lhs + i], indent + 2);
        }
        sprintf(buffer + strlen(buffer), "%*s]", indent, "");
    }

    sprintf(buffer + strlen(buffer), ")");
}

//endregion


//region Special

void ast_debug_fn_proto(char *buffer, Ast *ast, AstIndex index, AstNode *node, Token *main_token, int indent) {
    AstFnProto proto = *((AstFnProto*) &ast->extra_data.data[node->data.lhs]);

    sprintf(buffer + strlen(buffer), "{ ");

    // Append params
    sprintf(buffer + strlen(buffer), "params = ");
    if (proto.param_start == ast_index_empty) {
        sprintf(buffer + strlen(buffer), "_");
    } else {
        sprintf(buffer + strlen(buffer), "[\n");
        size_t param_count = proto.param_end - proto.param_start + 1;
        for (uint32_t i = 0; i < param_count; i++) {
            ast_debug_print_node(buffer, ast, proto.param_start + i, indent + 2);
        }
        sprintf(buffer + strlen(buffer), "%*s]", indent, "");
    }

    // Append return type expr
    sprintf(buffer + strlen(buffer), ", ret = ");
    if (node->data.rhs == ast_index_empty) {
        sprintf(buffer + strlen(buffer), "_");
    } else {
        //todo
        assert(false);
    }

    sprintf(buffer + strlen(buffer), " }");
}

void ast_debug_fn_param(char *buffer, Ast *ast, AstIndex index, AstNode *node, Token *main_token, int indent) {
    sprintf(buffer + strlen(buffer), "%*s", indent, "");

    sprintf(buffer + strlen(buffer), "param(");
    ast_debug_append_token_content(buffer, ast, node->main_token);

    sprintf(buffer + strlen(buffer), ", type = ");
    if (node->data.rhs == ast_index_empty) {
        sprintf(buffer + strlen(buffer), "_");
    } else {
        //todo
        assert(false);
    }
    sprintf(buffer + strlen(buffer), "),");
}

void ast_debug_field(char *buffer, Ast *ast, AstIndex index, AstNode *node, Token *main_token, int indent) {
    sprintf(buffer + strlen(buffer), "%*s", indent, "");

    sprintf(buffer + strlen(buffer), "field(");
    ast_debug_append_token_content(buffer, ast, node->main_token);

    sprintf(buffer + strlen(buffer), ", type = ");
    if (node->data.rhs == ast_index_empty) {
        sprintf(buffer + strlen(buffer), "_");
    } else {
        //todo
        assert(false);
    }
    sprintf(buffer + strlen(buffer), "),");
}

void ast_debug_enum_case(char *buffer, Ast *ast, AstIndex index, AstNode *node, Token *main_token, int indent) {
    sprintf(buffer + strlen(buffer), "%*s", indent, "");

    sprintf(buffer + strlen(buffer), "case(");
    ast_debug_append_token_content(buffer, ast, node->main_token);
    sprintf(buffer + strlen(buffer), "),");
}

//endregion

void ast_debug_module(char *buffer, Ast *ast, AstIndex index, AstNode *node, Token *main_token, int indent) {
    sprintf(buffer + strlen(buffer), "// module\n");

    if (node->data.lhs != ast_index_empty) {
        AstIndex start = node->data.lhs;
        uint32_t expr_count = node->data.rhs - start + 1;

        for (uint32_t i = 0; i < expr_count; i++) {
            sprintf(buffer + strlen(buffer), "\n%*s// @%d\n", indent, "", i + 1);
            ast_debug_print_node(buffer, ast, ast->extra_data.data[start + i], indent);
        }
    }
}

void ast_debug_empty(char *buffer, Ast *ast, AstIndex index, AstNode *node, Token *main_token, int indent) {
    sprintf(buffer + strlen(buffer), "%*s", indent, "");
    sprintf(buffer + strlen(buffer), "%%%d = <empty>", index);
}

AstDebugFn ast_debug_fns[__AST_LAST] = {
    ast_debug_literal_generic,  // int
    ast_debug_literal_generic,  // bool
    ast_debug_literal_generic,  // ref
    ast_debug_binary,           // binary
    ast_debug_unary,            // unary
    ast_debug_block,            // block
    ast_debug_return,           // return
    ast_debug_if,               // if
    ast_debug_while,            // while
    ast_debug_binary,           // dot
    ast_debug_call,             // call

    ast_debug_stmt_let,         // let

    ast_debug_fn_named,         // fn_named
    ast_debug_struct,           // struct
    ast_debug_enum,             // enum

    ast_debug_fn_proto,         // fn_proto
    ast_debug_fn_param,         // fn_param
    ast_debug_field,            // field
    ast_debug_enum_case,        // enum_case

    ast_debug_module,           // module
    ast_debug_empty,            // empty
};

char *ast_debug_print(Ast *ast) {
    if (ast->root == ast_index_empty) {
        return "EMPTY AST\n";
    }

    char *buffer = malloc(4096);
    memset(buffer, 0, 4096);

    ast_debug_print_node(buffer, ast, ast->root, 0);

    return buffer;
}

void ast_debug_print_node_raw(char *buffer, Ast *ast, AstIndex index, int indent) {
    AstNode *node = &ast->nodes.data[index];
    Token *main_token = &ast->tokens.data[node->main_token];
    ast_debug_fns[node->tag](buffer, ast, index, node, main_token, indent);
}

void ast_debug_print_node(char *buffer, Ast *ast, AstIndex index, int indent) {
    ast_debug_print_node_raw(buffer, ast, index, indent);

    //todo should add an offset to ast that covers the entire portion that the node takes up. For now this is really inaccurate for anything but numbers.
    //printf(" offset:%zu..%zu", main_token.loc.start - src_start, main_token.loc.end - src_start);
    sprintf(buffer + strlen(buffer), "\n");
}
