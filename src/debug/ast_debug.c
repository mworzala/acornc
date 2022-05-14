#include <printf.h>
#include "debug/ast_debug.h"

void ast_debug_print(Ast *ast) {
    if (ast->root == ast_index_empty) {
        printf("EMPTY AST\n");
        return;
    }

    for (size_t i = 0; i < ast->nodes.size; i++) {
        ast_debug_print_node(ast, i, 0);
    }
}

void ast_debug_print_node(Ast *ast, AstIndex index, int indent) {
    size_t src_start = (size_t) ast->source;
    AstNode *node = &ast->nodes.data[index];
    Token main_token = ast->tokens.data[node->main_token];

    printf("%*s", indent, "");
    printf("%%%d = ", index);

    switch (node->tag) {
        case AST_INTEGER:
            printf("int(%.*s)", (int)(main_token.loc.end - main_token.loc.start), src_start + (main_token.loc.start - src_start));
            break;
        case AST_UNARY:
            printf("%s(%%%d)", token_type_to_string(main_token.type), node->data.lhs);
            break;
        case AST_BINARY:
            printf("%s(%%%d, %%%d)", token_type_to_string(main_token.type), node->data.lhs, node->data.rhs);
            break;
        default:
            printf("UNKNOWN");
            break;
    }

    //todo should add an offset to ast that covers the entire portion that the node takes up. For now this is really inaccurate for anything but numbers.
    printf(" offset:%zu..%zu", main_token.loc.start - src_start, main_token.loc.end - src_start);
    printf("\n");
}
