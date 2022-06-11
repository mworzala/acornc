#include "../parse_test_check.h"

TEST(Parser, BasicReturn) {
    auto input = "return";
    auto expected = R"#(
ret
)#";
    EXPECT_EXPR(input, expected, false);
}

TEST(Parser, ReturnExpr) {
    auto input = "return 1";
    auto expected = R"#(
ret
  int "1"
)#";
    EXPECT_EXPR(input, expected, false);
}
