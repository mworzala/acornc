#include "parser.h"
#include "parser_internal.h"

#include <assert.h>


#define self_t Parser *self

void parser_init(self_t, uint8_t *source) {
    self->source = source;

    token_list_init(&self->tokens);
    self->tok_index = 0;

    Lexer lexer;
    lexer_init(&lexer, source);
    Token tok;
    while ((tok = lexer_next(&lexer)).type != TOK_EOF)
        token_list_insert(&self->tokens, tok);
    // Insert EOF token at end
    token_list_insert(&self->tokens, tok);

    ast_node_list_init(&self->nodes);
    ast_index_list_init(&self->extra_data);
}

Ast parser_parse(self_t) {
    expr_bp(self);

    return (Ast) {
        .source = self->source,
        .tokens = self->tokens,
        .nodes = self->nodes,
        .extra_data = self->extra_data,
    };
}

#undef self_t