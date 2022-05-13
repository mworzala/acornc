#ifndef ACORNC_LEXER_INTERNAL_H
#define ACORNC_LEXER_INTERNAL_H

#include "common.h"

typedef enum token_type_s TokenType;
typedef struct lexer_s Lexer;
typedef struct token_s Token;

// Character predicates
bool lex_is_digit(uint8_t c);
bool lex_is_alpha(uint8_t c);

#define self_t Lexer *self

// Scanning utilities
bool lex_at_end(self_t);
uint8_t lex_peek0(self_t);
uint8_t lex_peek1(self_t);
uint8_t lex_advance(self_t);
//uint8_t peek_next(self_t);

// Token factory
Token new_token(self_t, TokenType type);
Token new_token_error(self_t, const char *message);

// Complex lex rules

void lex_skip_trivia(self_t);
Token lex_number(self_t);
Token lex_symbol(self_t, uint8_t c);

#undef self_t

#endif //ACORNC_LEXER_INTERNAL_H
