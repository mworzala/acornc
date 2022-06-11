#include "../parse_test_check.h"

TEST(Parser, EmptyBlock) {
    auto input = "{}";
    auto expected = R"#(
block
)#";
    EXPECT_EXPR(input, expected, false);
}

TEST(Parser, SingleExprBlock) {
    auto input = "{1;}";
    auto expected = R"#(
block
  int "1"
)#";
    EXPECT_EXPR(input, expected, false);
}

TEST(Parser, MultiExprBlock) {
    auto input = "{1;2;}";
    auto expected = R"#(
block
  int "1"
  int "2"
)#";
    EXPECT_EXPR(input, expected, false);
}

TEST(Parser, BlockAllowNoFinalSemicolon) {
    //todo this is actually syntax sugar for a return statement. Need to add that either here or when lowering. Probably lowering
    auto input = "{1;2}";
    auto expected = R"#(
block
  int "1"
  int "2"
)#";
    EXPECT_EXPR(input, expected, false);
}

TEST(Parser, BlockAcceptExprOrStmt) {
    auto input = "{let x; x}";
    auto expected = R"#(
block
  let "x"
  ref "x"
)#";
    EXPECT_EXPR(input, expected, false);
}


// Regressions


TEST(Parser, BlockWithinBlock) {
    auto input = "{{x}}";
    auto expected = R"#(
block
  block
    ref "x"
)#";
    EXPECT_EXPR(input, expected, false);
}
