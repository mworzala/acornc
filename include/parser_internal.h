#ifndef ACORNC_PARSER_INTERNAL_H
#define ACORNC_PARSER_INTERNAL_H

#include "common.h"
#include "lexer.h"
#include "ast.h"

typedef struct parser_s Parser;

#define self_t Parser *self

// Parser utilities
Token parse_peek_last(self_t);
Token parse_peek_curr(self_t);
Token parse_advance(self_t);


// Pratt BP
typedef struct {
    uint8_t lhs;
    uint8_t rhs;
} BindingPower;

BindingPower token_infix_bp(Token token);


// Parse functions
TokenIndex expr_bp(self_t, uint8_t min_bp);
TokenIndex expr_number(self_t);


#undef self_t

#endif //ACORNC_PARSER_INTERNAL_H
