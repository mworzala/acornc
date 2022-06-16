#include "../parse_test_check.h"

TEST(Hir, SmallInt) {
    auto input = "1";
    auto expected = R"#(
%0 = int(1)
)#";
    EXPECT_EXPR(input, expected);
}

TEST(Hir, String) {
    auto input = "\"Hello, world!\"";
    auto expected = R"#(
%0 = str("Hello, world!")
)#";
    EXPECT_EXPR(input, expected);
}

TEST(Hir, Bool) {
    auto input = "true";
    auto expected = R"#(
%0 = bool(true)
)#";
    EXPECT_EXPR(input, expected);
}
