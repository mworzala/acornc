#include "../parse_test_check.h"

TEST(Parser, BasicReturn) {
    auto input = "return";
    auto expected = R"#(
%0 = ret
)#";
    EXPECT_EXPR(input, expected);
}

TEST(Parser, ReturnExpr) {
    auto input = "return 1";
    auto expected = R"#(
%0 = int(1)
%1 = ret(%0)
)#";
    EXPECT_EXPR(input, expected);
}
