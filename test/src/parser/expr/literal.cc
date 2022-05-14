#include "../parse_test_check.h"

TEST(Parser, Number) {
    auto input = "10";
    auto expected = R"#(
%0 = int(10)
)#";
    EXPECT_EXPR(input, expected);
}

TEST(Parser, NumberProceededByWhitespace) {
    auto input = "  10";
    auto expected = R"#(
%0 = int(10)
)#";
    EXPECT_EXPR(input, expected);
}

TEST(Parser, NumberFollowedByWhitespace) {
    auto input = "10  ";
    auto expected = R"#(
%0 = int(10)
)#";
    EXPECT_EXPR(input, expected);
}

TEST(Parser, NumberSurroundedByWhitespace) {
    auto input = "  10  ";
    auto expected = R"#(
%0 = int(10)
)#";
    EXPECT_EXPR(input, expected);
}

TEST(Parser, NumberProceededByComment) {
    auto input = "//comment\n10";
    auto expected = R"#(
%0 = int(10)
)#";
    EXPECT_EXPR(input, expected);
}

TEST(Parser, NumberFollowedByComment) {
    auto input = "10\n//Comment";
    auto expected = R"#(
%0 = int(10)
)#";
    EXPECT_EXPR(input, expected);
}

TEST(Parser, LiteralTrue) {
    auto input = "true";
    auto expected = R"#(
%0 = bool(true)
)#";
    EXPECT_EXPR(input, expected);
}

TEST(Parser, LiteralFalse) {
    auto input = "false";
    auto expected = R"#(
%0 = bool(false)
)#";
    EXPECT_EXPR(input, expected);
}
