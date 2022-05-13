#include "parser_internal.h"

#include <assert.h>

#include "parser.h"

// Empty values

static const AstNode ast_node_empty = {
    .tag = AST_EMPTY,
    .main_token = UINT32_MAX,
    .data = {
        .lhs = ast_index_empty,
        .rhs = ast_index_empty,
    },
};

static const AstData ast_data_empty = {
    .lhs = ast_index_empty,
    .rhs = ast_index_empty,
};

#define self_t Parser *self

Token parse_peek_last(self_t) {
    assert(self->tok_index > 0);
    return self->tokens.data[self->tok_index - 1];
}

Token parse_peek_curr(self_t) {
    return self->tokens.data[self->tok_index];
}

Token parse_advance(self_t) {
    if (self->tokens.size <= self->tok_index + 1) {
        // Always return last element, which is known to be EOF.
        return self->tokens.data[self->tokens.size - 1];
    }

    Token tok = parse_peek_curr(self);
    self->tok_index++;
    return tok;
}


// Parse functions

TokenIndex expr_bp(self_t, uint8_t min_bp) {
    Token next = parse_advance(self);

    TokenIndex lhs;
    if (next.type == TOK_NUMBER) {
        lhs = expr_number(self);
    } else {
        return ast_index_empty;
    }

    for (;;) {
        BindingPower bp = token_infix_bp(parse_peek_curr(self));
        if (bp.lhs == 0) break;

        if (bp.lhs < min_bp) break;

        TokenIndex op_idx = self->tok_index;
        parse_advance(self); // Eat the operator symbol

        TokenIndex rhs = expr_bp(self, bp.rhs);

        ast_node_list_add(&self->nodes, (AstNode) {
            .tag = AST_BINARY,
            .main_token = op_idx,
            .data = { lhs, rhs },
        });
        lhs = self->nodes.size - 1;

        //todo
    }

    return lhs;
}

TokenIndex expr_number(self_t) {
    ast_node_list_add(&self->nodes, (AstNode) {
        .tag = AST_INTEGER,
        .main_token = self->tok_index - 1,
        .data = ast_data_empty,
    });
    return self->nodes.size - 1;
}


// Pratt BP

BindingPower token_infix_bp(Token token) {
    switch (token.type) {
        case TOK_PLUS:
        case TOK_MINUS:
            return (BindingPower) {1, 2};
        case TOK_STAR:
        case TOK_SLASH:
            return (BindingPower) {3, 4};
        default:
            return (BindingPower) {0, 0};
    }
}


#undef self_t

























