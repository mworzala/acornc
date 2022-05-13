#ifndef ACORNC_LEXER_H
#define ACORNC_LEXER_H

#include "common.h"

typedef enum token_type_s {
    // Symbols
    TOK_LPAREN, TOK_RPAREN,
    TOK_MINUS, TOK_PLUS, TOK_STAR, TOK_SLASH,

    // Keywords

    // Literals
    TOK_NUMBER,

    // Special
    TOK_ERROR, TOK_EOF,
} TokenType;

const char *token_type_to_string(TokenType type);

typedef struct loc_s {
    size_t start;
    size_t end;
} Loc;

typedef struct token_s {
    TokenType type;
    Loc loc;
} Token;

typedef struct lexer_s {
    const uint8_t *start;
    const uint8_t *current;
} Lexer;

#define self_t Lexer *self

void lexer_init(self_t, const uint8_t *source);
Token lexer_next(self_t);

#undef self_t

#endif //ACORNC_LEXER_H
