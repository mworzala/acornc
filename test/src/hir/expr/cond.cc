#include "../parse_test_check.h"

TEST(Hir, BasicCond) {
    auto input = R"#(
if true { }
)#";
    auto expected = R"#(
%1 = bool(true)
%2 = block()
%0 = cond(%1, %2, _)
)#";
    EXPECT_EXPR(input, expected);
}

TEST(Hir, CondWithElseBranch) {
    auto input = R"#(
if true { } else { }
)#";
    auto expected = R"#(
%1 = bool(true)
%2 = block()
%3 = block()
%0 = cond(%1, %2, %3)
)#";
    EXPECT_EXPR(input, expected);
}

TEST(Hir, CondChain) {
    auto input = R"#(
if true { } else if false { }
)#";
    auto expected = R"#(
%1 = bool(true)
%2 = block()
%4 = bool(false)
%5 = block()
%3 = cond(%4, %5, _)
%0 = cond(%1, %2, %3)
)#";
    EXPECT_EXPR(input, expected);
}

TEST(Hir, CondChainWithElse) {
    auto input = R"#(
if true { } else if false { } else { }
)#";
    auto expected = R"#(
%1 = bool(true)
%2 = block()
%4 = bool(false)
%5 = block()
%6 = block()
%3 = cond(%4, %5, %6)
%0 = cond(%1, %2, %3)
)#";
    EXPECT_EXPR(input, expected);
}

TEST(Hir, CondWithResultExpr) {
    auto input = R"#(
if true {
    1
} else {
    2
}
)#";
    auto expected = R"#(
%1 = bool(true)
%2 = block({
  %4 = int(1)
  %3 = break_inline(%4)
})
%5 = block({
  %7 = int(2)
  %6 = break_inline(%7)
})
%0 = cond(%1, %2, %5)
)#";
    EXPECT_EXPR(input, expected);
}
