#include <gtest/gtest.h>

extern "C" {
#include "lexer.h"
}

testing::AssertionResult check(const char *input, TokenType expected) {
    Lexer lexer;
    lexer_init(&lexer, reinterpret_cast<const uint8_t *>(input));
    Token token = lexer_next(&lexer);
    if (token.type != expected) {
        return testing::AssertionFailure()
            << "Expected "
            << token_type_to_string(expected)
            << " but got "
            << token_type_to_string(token.type);
    }
    Token eof = lexer_next(&lexer);
    if (eof.type != TOK_EOF) {
        return testing::AssertionFailure()
            << "Expected EOF but got "
            << token_type_to_string(eof.type);
    }

    return testing::AssertionSuccess();
}

TEST(Lexer, SingleTokensSymbols) {
    EXPECT_PRED2(check, "(", TOK_LPAREN);
    EXPECT_PRED2(check, ")", TOK_RPAREN);
    EXPECT_PRED2(check, "{", TOK_LBRACE);
    EXPECT_PRED2(check, "}", TOK_RBRACE);
    EXPECT_PRED2(check, "-", TOK_MINUS);
    EXPECT_PRED2(check, "+", TOK_PLUS);
    EXPECT_PRED2(check, "*", TOK_STAR);
    EXPECT_PRED2(check, "/", TOK_SLASH);
    EXPECT_PRED2(check, "=", TOK_EQ);
    EXPECT_PRED2(check, "==", TOK_EQEQ);
    EXPECT_PRED2(check, "!", TOK_BANG);
    EXPECT_PRED2(check, "!=", TOK_BANGEQ);
    EXPECT_PRED2(check, "<", TOK_LT);
    EXPECT_PRED2(check, "<=", TOK_LTEQ);
    EXPECT_PRED2(check, ">", TOK_GT);
    EXPECT_PRED2(check, ">=", TOK_GTEQ);
    EXPECT_PRED2(check, "&&", TOK_AMPAMP);
    EXPECT_PRED2(check, "||", TOK_BARBAR);

    // Sanity check of some hardcoded errors.
    EXPECT_PRED2(check, "&", TOK_ERROR);
    EXPECT_PRED2(check, "|", TOK_ERROR);
}

TEST(Lexer, SingleTokensKeywords) {
    EXPECT_PRED2(check, "let", TOK_LET);
}

TEST(Lexer, SingleTokensLiteralNumber) {
    EXPECT_PRED2(check, "1", TOK_NUMBER);
    EXPECT_PRED2(check, "123", TOK_NUMBER);
    EXPECT_PRED2(check, "12.3", TOK_NUMBER);
}

TEST(Lexer, SingleTokensLiteralIdent) {
    EXPECT_PRED2(check, "a", TOK_IDENT);
    EXPECT_PRED2(check, "aa", TOK_IDENT);
    EXPECT_PRED2(check, "a12b", TOK_IDENT);
    EXPECT_PRED2(check, "a1_2b_", TOK_IDENT);
}

TEST(Lexer, SingleTokensLiteralBool) {
    EXPECT_PRED2(check, "true", TOK_TRUE);
    EXPECT_PRED2(check, "false", TOK_FALSE);
}
