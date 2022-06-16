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

TEST(Parser, AllOpsPrecedence) {
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
