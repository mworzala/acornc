#include "lexer.h"
#include "lexer_internal.h"

#define self_t Lexer *self

void lexer_init(self_t, const uint8_t *source) {
    self->start = source;
    self->current = source;
}

Token lexer_next(self_t) {
    skip_trivia(self);
    self->start = self->current;

    if (at_end(self)) {
        return new_token(self, TOK_EOF);
    }

    uint8_t c = advance(self);

//    if (is_alpha(self,c))
//        return lex_ident(self);
    if (is_digit(c))
        return lex_number(self);
//    if (c == '"')
//        return lex_string(self);

    return lex_symbol(self, c);
}

#undef self_t


const char *token_type_to_string(TokenType type) {
    switch (type) {
        case TOK_LPAREN:
            return "LPAREN";
        case TOK_RPAREN:
            return "RPAREN";
        case TOK_MINUS:
            return "MINUS";
        case TOK_PLUS:
            return "PLUS";
        case TOK_STAR:
            return "STAR";
        case TOK_SLASH:
            return "SLASH";
        case TOK_NUMBER:
            return "NUMBER";
        case TOK_ERROR:
            return "<err>";
        case TOK_EOF:
            return "<eof>";
        default:
            return "<?>";
    }
}

