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
                } else return; // not a comment, parse the slash normally
            default:
                return;
        }
    }
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
        case '(':
            return new_token(self, TOK_LPAREN);
        case ')':
            return new_token(self, TOK_RPAREN);
        case '-':
            return new_token(self, TOK_MINUS);
        case '+':
            return new_token(self, TOK_PLUS);
        case '*':
            return new_token(self, TOK_STAR);
        case '/':
            return new_token(self, TOK_SLASH);
        case '!':
            return new_token(self, TOK_BANG);
        default:
            return new_token_error(self, "Unknown symbol");
    }
}

#undef self_t
