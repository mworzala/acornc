#include <gtest/gtest.h>

extern "C" {
#include "lexer.h"
}

testing::AssertionResult lex_reg_check(const char *input, const char *expected) {
    Lexer lexer;
    lexer_init(&lexer, (uint8_t *) input);

    std::stringstream actual_ss;

    Token token;
    while ((token = lexer_next(&lexer)).type != TOK_EOF) {
        char buf[32];
        memset(buf, 0, 32);
        memcpy(buf, (char *) (input + token.loc.start), token.loc.end - token.loc.start);
        actual_ss << token_type_to_string(token.type) << "~" << buf << " ";
    }

    std::string actual_s = actual_ss.str();
    char *actual = const_cast<char *>(actual_s.c_str());
    actual[strlen(actual) - 1] = '\0';

    if (strcmp(actual, expected) == 0) {
        return testing::AssertionSuccess();
    } else {
        printf("Expected:\n%s\n", expected);
        printf("Actual:\n%s\n", actual);
        return testing::AssertionFailure();
    }
}

#define CHECK(input, expected) EXPECT_PRED2(lex_reg_check, input, expected)

TEST(Lexer, LexNumberFollowingComment) {
    CHECK("// comment\n10", "NUMBER~10");
}

TEST(Lexer, LexTwoDifferentWords) {
    CHECK("false let", "BOOLEAN~false LET~let");
}

TEST(Lexer, StringLiteralAcrossNewline) {
    EXPECT_DEATH({
        CHECK("\"Hello, \nWorld\"", "STRING~Hello, World");
    }, "Assertion failed: \\(false\\), function lex_string, file lexer_internal\\.c");
}


