#ifndef ACORNC_PARSER_H
#define ACORNC_PARSER_H

#include "common.h"
#include "lexer.h"
#include "ast.h"
#include "array_util.h"

typedef struct parser_s {
    uint8_t *source;

    TokenList tokens;
    uint32_t tok_index;

    //todo errors
    AstNodeList nodes;
    IndexList extra_data;
} Parser;

#define self_t Parser *self

void parser_init(self_t, uint8_t *source);
Ast parser_parse(self_t);

#undef self_t

#endif //ACORNC_PARSER_H
