#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"


//todo there should probably be an AST_PAREN to account for them in the ast.
//     I dont think the AST should be in charge of removing syntax sugar, that should be when lowering it to HIR.
const char *ast_tag_to_string(AstTag tag) {
    switch (tag) {
        case AST_INTEGER:
            return "int";
        case AST_STRING:
            return "str";
        case AST_BOOL:
            return "bool";
        case AST_REF:
            return "ref";
        case AST_BINARY:
            return "binary";
        case AST_UNARY:
            return "unary";
        case AST_BLOCK:
            return "block";
        case AST_RETURN:
            return "ret";
        case AST_IF:
            return "if";
        case AST_WHILE:
            return "while";
        case AST_DOT:
            return "dot";
        case AST_CALL:
            return "call";

        case AST_LET:
            return "let";

        case AST_CONST:
            return "const";
        case AST_NAMED_FN:
            return "named_fn";
        case AST_STRUCT:
            return "struct";
        case AST_ENUM:
            return "enum";

        case AST_TYPE:
            return "type";

        case AST_FN_PROTO:
            return "fn_proto";
        case AST_FN_PARAM:
            return "fn_param";
        case AST_FIELD:
            return "field";
        case AST_ENUM_CASE:
            return "enum_case";

        case AST_MODULE:
            return "module";
        case AST_EMPTY:
            return "<empty>";
        case AST_ERROR:
            return "error";
        default:
            return "<?>";
    }
}

#define self_t AstNodeList *self

void ast_node_list_init(self_t) {
    self->size = 0;
    self->capacity = 0;
    self->data = NULL;
}

void ast_node_list_free(self_t) {
    ARRAY_FREE(AstNode, self->data);
    ast_node_list_init(self);
}

void ast_node_list_add(self_t, AstNode node) {
    if (self->capacity < self->size + 1) {
        self->capacity = ARRAY_GROW_CAPCITY(self->capacity);
        self->data = ARRAY_GROW(AstNode, self->data, self->capacity);
    }

    self->data[self->size] = node;
    self->size++;
}

#undef self_t

#define self_t Ast *self

AstNode *ast_get_node(self_t, AstIndex index) {
    return &self->nodes.data[index];
}

AstNode *ast_get_node_tagged(self_t, AstIndex index, AstTag tag) {
    AstNode *node = ast_get_node(self, index);
    assert(tag == node->tag);
    return node;
}

char *ast_get_token_content(self_t, TokenIndex token) {
    Token main_token = self->tokens.data[token];
    size_t str_len = main_token.loc.end - main_token.loc.start;
    char *str = malloc(str_len + 1);
    memcpy(str, (const void *) self->source + main_token.loc.start, str_len);
    str[str_len] = '\0';
    return str;
}

#undef self_t

char *ast_error_to_string(AstError error) {
    switch (error) {
        case AST_ERR_UNEXPECTED_EOF:
            return "unexpected end of file";
        case AST_ERR_EXPECTED_EXPRESSION:
            return "expected expression, found ...";
        case AST_ERR_MISSING_SEMICOLON:
            return "missing semicolon";
        default:
            return "unknown error";
    }
}
