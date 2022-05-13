#include <gtest/gtest.h>
#include "lexer.h"

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

TEST(Lexer, SingleTokens) {
    EXPECT_PRED2(check, "(", TOK_LPAREN);
}