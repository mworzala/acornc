#ifndef ACORNC_LEXER_H
#define ACORNC_LEXER_H

#include "common.h"

// Represents an index into the token array for the contained file.
typedef uint32_t TokenIndex;

typedef enum token_type_s {
    // Symbols
    TOK_LPAREN, TOK_RPAREN,
    TOK_LBRACE, TOK_RBRACE,
    TOK_MINUS, TOK_PLUS, TOK_STAR, TOK_SLASH,
    TOK_EQ, TOK_EQEQ,
    TOK_BANG, TOK_BANGEQ,
    TOK_LT, TOK_LTEQ,
    TOK_GT, TOK_GTEQ,
    TOK_AMPAMP, TOK_BARBAR,
    TOK_SEMI, TOK_COLON, TOK_COMMA, TOK_DOT,

    // Keywords
    TOK_ELSE, TOK_ENUM, TOK_FN,
    TOK_FOREIGN, TOK_IF, TOK_LET,
    TOK_RETURN, TOK_STRUCT, TOK_WHILE,

    // Literals
    TOK_NUMBER, TOK_STRING, TOK_IDENT, TOK_TRUE, TOK_FALSE,

    // Special
    TOK_ERROR, TOK_EOF,
    __TOK_LAST,
} TokenType;

const char *token_type_to_string(TokenType type);

#undef self_t

typedef struct token_loc_s {
    size_t start;
    size_t end;
} TokenLoc;

typedef struct token_s {
    TokenType type;
    TokenLoc loc;
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
