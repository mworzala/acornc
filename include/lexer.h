#ifndef ACORNC_LEXER_H
#define ACORNC_LEXER_H

#include "common.h"

typedef enum token_type_s {
    // Symbols
    TOK_LPAREN, TOK_RPAREN,
    TOK_MINUS, TOK_PLUS, TOK_STAR, TOK_SLASH,
    TOK_EQ, TOK_EQEQ,
    TOK_BANG, TOK_BANGEQ,
    TOK_LT, TOK_LTEQ,
    TOK_GT, TOK_GTEQ,
    TOK_AMPAMP, TOK_BARBAR,

    // Keywords

    // Literals
    TOK_NUMBER, TOK_IDENT, TOK_TRUE, TOK_FALSE,

    // Special
    TOK_ERROR, TOK_EOF,
    __TOK_LAST,
} TokenType;

const char *token_type_to_string(TokenType type);

#undef self_t

typedef struct loc_s {
    size_t start;
    size_t end;
} Loc;

typedef struct token_s {
    TokenType type;
    Loc loc;
} Token;

typedef struct token_list_s {
    uint32_t size;
    uint32_t capacity;
    Token *data;
} TokenList;

//todo test token list
#define self_t TokenList *self

void token_list_init(self_t);
void token_list_free(self_t);
void token_list_insert(self_t, Token token);

#undef self_t

typedef struct lexer_s {
    const uint8_t *start;
    const uint8_t *current;
} Lexer;

#define self_t Lexer *self

void lexer_init(self_t, const uint8_t *source);
Token lexer_next(self_t);

#undef self_t

#endif //ACORNC_LEXER_H
