#include <gtest/gtest.h>

extern "C" {
#include "lexer.h"
}

TEST(Lexer, EnsureStringCaseForAllTokens) {
    for (int i = 0; i < __TOK_LAST; i++) {
        EXPECT_STRNE("<?>", token_type_to_string(static_cast<TokenType>(i)));
    }
}
