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
            .start = (size_t) self->start - self->origin,
            .end = (size_t) self->current - self->origin,
        }
    };
}

Token new_token_error(self_t, const char *message) {
    //todo need to include the error message somewhere.
    return (Token) {
        .type = TOK_ERROR,
        .loc = {
            .start = (size_t) self->start - self->origin,
            .end = (size_t) self->current - self->origin,
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

Token lex_string(self_t) {
    uint8_t next;
    while ((next = lex_peek0(self)) != '"' && !lex_at_end(self)) {
        if (next == '\n') assert(false);
        lex_advance(self);
    }

    if (lex_at_end(self)) assert(false); // Unterminated string

    lex_advance(self); // Eat the closing quote
    return new_token(self, TOK_STRING);
}

Token lex_symbol(self_t, uint8_t c) {
    switch (c) {
        // @formatter:off
        case '(':   return new_token(self, TOK_LPAREN);
        case ')':   return new_token(self, TOK_RPAREN);
        case '{':   return new_token(self, TOK_LBRACE);
        case '}':   return new_token(self, TOK_RBRACE);
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
        case ';':   return new_token(self, TOK_SEMI);
        case ':':   return new_token(self, TOK_COLON);
        case ',':   return new_token(self, TOK_COMMA);
        case '.':   return new_token(self, TOK_DOT);
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
 * const
 * else
 *  num
 * false
 *  n
 *  oreign
 * if
 * let
 * return
 * struct
 * true
 * while
 */
TokenType lex_ident_or_keyword(self_t) {
    // @formatter:off
    switch (self->start[0]) {
        case 'c': return check_keyword(self, 1, 4, "onst", TOK_CONST);
        case 'e':
            if (self->current - self->start > 1) {
                switch (self->start[1]) {
                    case 'l': return check_keyword(self, 2, 2, "se", TOK_ELSE);
                    case 'n': return check_keyword(self, 2, 2, "um", TOK_ENUM);
                }
            }
            break;
        case 'f':
            if (self->current - self->start > 1) {
                switch (self->start[1]) {
                    case 'a': return check_keyword(self, 2, 3, "lse", TOK_FALSE);
                    case 'n': return check_keyword(self, 2, 0, "", TOK_FN);
                    case 'o': return check_keyword(self, 2, 5, "reign", TOK_FOREIGN);
                }
            }
            break;
        case 'i': return check_keyword(self, 1, 1, "f", TOK_IF);
        case 'l': return check_keyword(self, 1, 2, "et", TOK_LET);
        case 'r': return check_keyword(self, 1, 5, "eturn", TOK_RETURN);
        case 's': return check_keyword(self, 1, 5, "truct", TOK_STRUCT);
        case 't': return check_keyword(self, 1, 3, "rue", TOK_TRUE);
        case 'w': return check_keyword(self, 1, 4, "hile", TOK_WHILE);
    }
    // @formatter:on
    return TOK_IDENT;
}

#undef self_t
