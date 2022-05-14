#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "debug/ast_debug.h"

char *ast_debug_print(Ast *ast) {
    if (ast->root == ast_index_empty) {
        return "EMPTY AST\n";
    }

    char *buffer = malloc(4096);
    memset(buffer, 0, 4096);

    for (size_t i = 0; i < ast->nodes.size; i++) {
        ast_debug_print_node(buffer, ast, i, 0);
    }

    return buffer;
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
        default:
            return "<?>";
    }
}

void ast_debug_print_node(char *buffer, Ast *ast, AstIndex index, int indent) {
    size_t src_start = (size_t) ast->source;
    AstNode *node = &ast->nodes.data[index];
    Token main_token = ast->tokens.data[node->main_token];

    sprintf(buffer + strlen(buffer), "%*s", indent, "");
    sprintf(buffer + strlen(buffer), "%%%d = ", index);

    switch (node->tag) {
        case AST_INTEGER:
            sprintf(buffer + strlen(buffer), "int(%.*s)", (int)(main_token.loc.end - main_token.loc.start), src_start + (main_token.loc.start - src_start));
            break;
        case AST_UNARY:
            sprintf(buffer + strlen(buffer), "%s(%%%d)", unary_op_token_to_string(main_token.type), node->data.lhs);
            break;
        case AST_BINARY:
            sprintf(buffer + strlen(buffer), "%s(%%%d, %%%d)", binary_op_token_to_string(main_token.type), node->data.lhs, node->data.rhs);
            break;
        default:
            sprintf(buffer + strlen(buffer), "UNKNOWN");
            break;
    }

    //todo should add an offset to ast that covers the entire portion that the node takes up. For now this is really inaccurate for anything but numbers.
    //printf(" offset:%zu..%zu", main_token.loc.start - src_start, main_token.loc.end - src_start);
    sprintf(buffer + strlen(buffer), "\n");
}
