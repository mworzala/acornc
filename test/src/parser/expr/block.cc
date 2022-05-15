#include "../parse_test_check.h"

TEST(Parser, EmptyBlock) {
    auto input = "{}";
    auto expected = R"#(
%0 = block(stmts = _)
)#";
    EXPECT_EXPR(input, expected);
}

TEST(Parser, SingleExprBlock) {
    auto input = "{1}";
    auto expected = R"#(
%1 = block(stmts = {
  %0 = int(1)
})
)#";
    EXPECT_EXPR(input, expected);
}

//todo semicolon between all but last
TEST(Parser, MultiExprBlock) {
    auto input = "{1 2}";
    auto expected = R"#(
%1 = block(stmts = {
  // @1
  %0 = int(1)
  // @2
  %2 = int(1)
})
)#";
    EXPECT_EXPR(input, expected);
}
