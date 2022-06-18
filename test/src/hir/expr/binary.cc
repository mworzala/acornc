#include "../parse_test_check.h"

TEST(Hir, BinAdd) {
    auto input = "1+2";
    auto expected = R"#(
%1 = int(1)
%2 = int(2)
%0 = add(%1, %2)
)#";
    EXPECT_EXPR(input, expected);
}

TEST(Hir, BinSub) {
    auto input = "1-2";
    auto expected = R"#(
%1 = int(1)
%2 = int(2)
%0 = sub(%1, %2)
)#";
    EXPECT_EXPR(input, expected);
}

TEST(Hir, BinMul) {
    auto input = "1*2";
    auto expected = R"#(
%1 = int(1)
%2 = int(2)
%0 = mul(%1, %2)
)#";
    EXPECT_EXPR(input, expected);
}

TEST(Hir, BinDiv) {
    auto input = "1/2";
    auto expected = R"#(
%1 = int(1)
%2 = int(2)
%0 = div(%1, %2)
)#";
    EXPECT_EXPR(input, expected);
}

TEST(Hir, AllOpsPrecedence) {
    auto input = "1+2*3/4-1";
    auto expected = R"#(
%2 = int(1)
%5 = int(2)
%6 = int(3)
%4 = mul(%5, %6)
%7 = int(4)
%3 = div(%4, %7)
%1 = add(%2, %3)
%8 = int(1)
%0 = sub(%1, %8)
)#";
    EXPECT_EXPR(input, expected);
}

TEST(Hir, BasicComparison) {
    auto input = "1 < 2 <= 3 > 4 >= 5 == 6 != 7";
    auto expected = R"#(
%6 = int(1)
%7 = int(2)
%5 = cmp_lt(%6, %7)
%8 = int(3)
%4 = cmp_le(%5, %8)
%9 = int(4)
%3 = cmp_gt(%4, %9)
%10 = int(5)
%2 = cmp_ge(%3, %10)
%11 = int(6)
%1 = cmp_eq(%2, %11)
%12 = int(7)
%0 = cmp_ne(%1, %12)
)#";
    EXPECT_EXPR(input, expected);
}

TEST(Hir, ComparisonChain) {
    //todo this lowering does not make sense
    // if i want to add chaining i need to do it correctly :)
    auto input = "1 < 2 < 3";
    auto expected = R"#(
%2 = int(1)
%3 = int(2)
%1 = cmp_lt(%2, %3)
%4 = int(3)
%0 = cmp_lt(%1, %4)
)#";
    EXPECT_EXPR(input, expected);
}

TEST(Hir, BasicLogical) {
    auto input = "true && false || true";
    auto expected = R"#(
%2 = bool(true)
%3 = bool(false)
%1 = and(%2, %3)
%4 = bool(true)
%0 = or(%1, %4)
)#";
    EXPECT_EXPR(input, expected);
}
