#include <string.h>
#include "lexer_internal.h"

#include "lexer.h"


// Character predicates
bool lex_is_digit(uint8_t c) {
    return c >= '0' && c <= '9';
}

bool lex_is_alpha(uint8_t c) {
    return c >= 'a' && c <= 'z' ||
           c >= 'A' && c <= 'Z' ||
           c == '_';
}


#define self_t Lexer *self

// Scanning utilities

bool lex_at_end(self_t) {
    return *self->current == '\0';
}

uint8_t lex_peek0(self_t) {
    return *self->current;
}

uint8_t lex_peek1(self_t) {
    if (lex_at_end(self)) return '\0';
    return *(self->current + 1);
}

uint8_t lex_advance(self_t) {
    self->current++;
    return self->current[-1];
}

bool lex_match(self_t, uint8_t c) {
    if (lex_at_end(self)) return false;
    if (*self->current != c) return false;
    self->current++;
    return true;
}


// Token factory

Token new_token(self_t, TokenType type) {
    return (Token) {
        .type = type,
        .loc = {
            .start = (size_t) self->start,
            .end = (size_t) self->current,
        }
    };
}

Token new_token_error(self_t, const char *message) {
    //todo need to include the error message somewhere.
    return (Token) {
        .type = TOK_ERROR,
        .loc = {
            .start = (size_t) self->start,
            .end = (size_t) self->current,
        },
    };
}

// Complex lex rules

void lex_skip_trivia(self_t) {
    for (;;) {
        uint8_t c = lex_peek0(self);
        switch (c) {
            case ' ':
            case '\t':
            case '\r':
            case '\n':
                lex_advance(self);
                break;
            case '/':
                // If there are two slashes, its a comment. Ignore until end of line.
                if (lex_peek1(self) == '/') {
                    while (lex_peek0(self) != '\n' && !lex_at_end(self))
                        lex_advance(self);
                    break;
                } else return; // not a comment, parse the slash normally
            default:
                return;
        }
    }
}

Token lex_ident(self_t) {
    while (lex_is_alpha(lex_peek0(self)) || lex_is_digit(lex_peek0(self)))
        lex_advance(self);
    return new_token(self, lex_ident_or_keyword(self));
}

Token lex_number(self_t) {
    // Read digits before decimal
    while (lex_is_digit(lex_peek0(self))) {
        lex_advance(self);
    }

    // Fractional section
    if (lex_peek0(self) == '.' && lex_is_digit(lex_peek1(self))) {
        lex_advance(self); // Eat the .

        // Consume the remaining digits
        while (lex_is_digit(lex_peek0(self))) {
            lex_advance(self);
        }
    }

    return new_token(self, TOK_NUMBER);
}

Token lex_symbol(self_t, uint8_t c) {
    switch (c) {
        // @formatter:off
        case '(':   return new_token(self, TOK_LPAREN);
        case ')':   return new_token(self, TOK_RPAREN);
        case '-':   return new_token(self, TOK_MINUS);
        case '+':   return new_token(self, TOK_PLUS);
        case '*':   return new_token(self, TOK_STAR);
        case '/':   return new_token(self, TOK_SLASH);
        case '=':   return new_token(self, lex_match(self, '=') ? TOK_EQEQ : TOK_EQ);
        case '!':   return new_token(self, lex_match(self, '=') ? TOK_BANGEQ : TOK_BANG);
        case '<':   return new_token(self, lex_match(self, '=') ? TOK_LTEQ : TOK_LT);
        case '>':   return new_token(self, lex_match(self, '=') ? TOK_GTEQ : TOK_GT);
        case '&':   return new_token(self, lex_match(self, '&') ? TOK_AMPAMP : TOK_ERROR);
        case '|':   return new_token(self, lex_match(self, '|') ? TOK_BARBAR : TOK_ERROR);
        default:    return new_token_error(self, "Unknown symbol");
        // @formatter:on
    }
}

// Helper for lex_ident_or_keyword to check if a remaining length matches a given string, otherwise its an identifier.
static TokenType check_keyword(self_t, int32_t start, int32_t length,
                               const char *rest, TokenType type) {
    if (self->current - self->start == start + length &&
        memcmp(self->start + start, rest, length) == 0) {
        return type;
    }

    return TOK_IDENT;
}

/*
 * Based on the following string table:
 * false
 * true
 */
TokenType lex_ident_or_keyword(self_t) {
    switch (self->start[0]) {
        case 'f':
            return check_keyword(self, 1, 4, "alse", TOK_FALSE);
        case 't':
            return check_keyword(self, 1, 3, "rue", TOK_TRUE);
        default:
            return TOK_IDENT;
    }
}

#undef self_t
