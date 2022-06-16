#include "../parse_test_check.h"

TEST(Hir, BlockEmpty) {
    auto input = "{}";
    auto expected = R"#(
%0 = block()
)#";
    EXPECT_EXPR(input, expected);
}

TEST(Hir, BlockNestedEmpty) {
    auto input = "{{}}";
    auto expected = R"#(
%0 = block({
  %1 = block()
})
)#";
    EXPECT_EXPR(input, expected);
}

TEST(Hir, BlockSingleExpr) {
    auto input = R"#(
{
  1;
}
)#";
    auto expected = R"#(
%0 = block({
  %1 = int(1)
})
)#";
    EXPECT_EXPR(input, expected);
}

TEST(Hir, BlockMultiExpr) {
    auto input = R"#(
{
  1;
  2;
}
)#";
    //todo would prefer something like below, but not sure how to do it
    // %0 = block({
    //   0: %1 = int(1)
    //   1: %2 = int(2)
    //})
    auto expected = R"#(
%0 = block({
  %1 = int(1)

  %2 = int(2)
})
)#";
    EXPECT_EXPR(input, expected);
}

TEST(Hir, BlockNontrivialStmt) {
    auto input = R"#(
{
  1 + 2;
  2 * 3;
}
)#";
    auto expected = R"#(
%0 = block({
  %2 = int(1)
  %3 = int(2)
  %1 = add(%2, %3)

  %5 = int(2)
  %6 = int(3)
  %4 = mul(%5, %6)
})
)#";
    EXPECT_EXPR(input, expected);
}
