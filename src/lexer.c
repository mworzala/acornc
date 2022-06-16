#include "lexer.h"
#include "lexer_internal.h"
#include "array_util.h"

#define self_t TokenList *self

void token_list_init(self_t) {
    self->size = 0;
    self->capacity = 0;
    self->data = NULL;
}

void token_list_free(self_t) {
    ARRAY_FREE(Token, self->data);
    token_list_init(self);
}

void token_list_insert(self_t, Token token) {
    if (self->capacity < self->size + 1) {
        self->capacity = ARRAY_GROW_CAPCITY(self->capacity);
        self->data = ARRAY_GROW(Token, self->data, self->capacity);
    }

    self->data[self->size] = token;
    self->size++;
}

#undef self_t

#define self_t Lexer *self

void lexer_init(self_t, const uint8_t *source) {
    self->origin = (size_t) source;
    self->start = source;
    self->current = source;
}

Token lexer_next(self_t) {
    lex_skip_trivia(self);
    self->start = self->current;

    if (lex_at_end(self)) {
        return new_token(self, TOK_EOF);
    }

    uint8_t c = lex_advance(self);

    if (lex_is_alpha(c))
        return lex_ident(self);
    if (lex_is_digit(c))
        return lex_number(self);
    if (c == '"')
        return lex_string(self);

    return lex_symbol(self, c);
}

#undef self_t


const char *token_type_to_string(TokenType type) {
    // @formatter:off
    switch (type) {
        case TOK_LPAREN:    return "LPAREN";
        case TOK_RPAREN:    return "RPAREN";
        case TOK_LBRACE:    return "LBRACE";
        case TOK_RBRACE:    return "RBRACE";
        case TOK_MINUS:     return "MINUS";
        case TOK_PLUS:      return "PLUS";
        case TOK_STAR:      return "STAR";
        case TOK_SLASH:     return "SLASH";
        case TOK_EQ:        return "EQ";
        case TOK_EQEQ:      return "EQEQ";
        case TOK_BANG:      return "BANG";
        case TOK_BANGEQ:    return "BANGEQ";
        case TOK_LT:        return "LT";
        case TOK_LTEQ:      return "LTEQ";
        case TOK_GT:        return "GT";
        case TOK_GTEQ:      return "GTEQ";
        case TOK_AMPAMP:    return "AMPAMP";
        case TOK_BARBAR:    return "BARBAR";
        case TOK_SEMI:      return "SEMI";
        case TOK_COLON:     return "COLON";
        case TOK_COMMA:     return "COMMA";
        case TOK_DOT:       return "DOT";

        case TOK_CONST:     return "CONST";
        case TOK_ELSE:      return "ELSE";
        case TOK_ENUM:      return "ENUM";
        case TOK_FN:        return "FN";
        case TOK_FOREIGN:   return "FOREIGN";
        case TOK_IF:        return "IF";
        case TOK_LET:       return "LET";
        case TOK_RETURN:    return "RETURN";
        case TOK_STRUCT:    return "STRUCT";
        case TOK_WHILE:     return "WHILE";

        case TOK_NUMBER:    return "NUMBER";
        case TOK_STRING:    return "STRING";
        case TOK_IDENT:     return "IDENT";
        case TOK_TRUE:
        case TOK_FALSE:     return "BOOLEAN";

        case TOK_ERROR:     return "<err>";
        case TOK_EOF:       return "<eof>";
        default:            return "<?>";
    }
    // @formatter:on
}

