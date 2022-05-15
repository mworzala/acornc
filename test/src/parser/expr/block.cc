#include "../parse_test_check.h"

TEST(Parser, EmptyBlock) {
    auto input = "{}";
    auto expected = R"#(
%0 = block(stmts = _)
)#";
    EXPECT_EXPR(input, expected);
}

TEST(Parser, SingleExprBlock) {
    auto input = "{1;}";
    auto expected = R"#(
%1 = block(stmts = {
  %0 = int(1)
})
)#";
    EXPECT_EXPR(input, expected);
}

TEST(Parser, MultiExprBlock) {
    auto input = "{1;2;}";
    auto expected = R"#(
%2 = block(stmts = {
  // @1
  %0 = int(1)
  // @2
  %1 = int(2)
})
)#";
    EXPECT_EXPR(input, expected);
}

TEST(Parser, BlockAllowNoFinalSemicolon) {
    auto input = "{1;2}";
    auto expected = R"#(
%2 = block(stmts = {
  // @1
  %0 = int(1)
  // @2
  %1 = int(2)
})
)#";
    EXPECT_EXPR(input, expected);
}

TEST(Parser, BlockAcceptExprOrStmt) {
    auto input = "{let x; x}";
    auto expected = R"#(
%2 = block(stmts = {
  // @1
  %0 = let(x, type = _, init = _)
  // @2
  %1 = ref(x)
})
)#";
    EXPECT_EXPR(input, expected);
}
