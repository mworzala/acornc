#include "../parse_test_check.h"

TEST(Parser, Number) {
    auto input = "10";
    auto expected = R"#(
int "10"
)#";
    EXPECT_EXPR(input, expected, false);
}

TEST(Parser, NumberProceededByWhitespace) {
    auto input = "  10";
    auto expected = R"#(
int "10"
)#";
    EXPECT_EXPR(input, expected, false);
}

TEST(Parser, NumberFollowedByWhitespace) {
    auto input = "10  ";
    auto expected = R"#(
int "10"
)#";
    EXPECT_EXPR(input, expected, false);
}

TEST(Parser, NumberSurroundedByWhitespace) {
    auto input = "  10  ";
    auto expected = R"#(
int "10"
)#";
    EXPECT_EXPR(input, expected, false);
}

TEST(Parser, NumberProceededByComment) {
    auto input = "//comment\n10";
    auto expected = R"#(
int "10"
)#";
    EXPECT_EXPR(input, expected, false);
}

TEST(Parser, NumberFollowedByComment) {
    auto input = "10\n//Comment";
    auto expected = R"#(
int "10"
)#";
    EXPECT_EXPR(input, expected, false);
}

TEST(Parser, LiteralString) {
    auto input = "\"Hello, World\"";
    auto expected = R"#(
str ""Hello, World""
)#";
    EXPECT_EXPR(input, expected, false);
}

TEST(Parser, LiteralTrue) {
    auto input = "true";
    auto expected = R"#(
bool "true"
)#";
    EXPECT_EXPR(input, expected, false);
}

TEST(Parser, LiteralFalse) {
    auto input = "false";
    auto expected = R"#(
bool "false"
)#";
    EXPECT_EXPR(input, expected, false);
}

TEST(Parser, LiteralRef) {
    auto input = "foo";
    auto expected = R"#(
ref "foo"
)#";
    EXPECT_EXPR(input, expected, false);
}
