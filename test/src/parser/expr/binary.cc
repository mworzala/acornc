#include "../parse_test_check.h"

TEST(Parser, SimpleAddMul) {
    auto input = "1+2*3";
    auto expected = R"#(
%0 = int(1)
%1 = int(2)
%2 = int(3)
%3 = mul(%1, %2)
%4 = add(%0, %3)
)#";
    EXPECT_EXPR(input, expected);
}

TEST(Parser, BasicOperators) {
    auto input = "1+2*3/4-1";
    auto expected = R"#(
%0 = int(1)
%1 = int(2)
%2 = int(3)
%3 = mul(%1, %2)
%4 = int(4)
%5 = div(%3, %4)
%6 = add(%0, %5)
%7 = int(1)
%8 = sub(%6, %7)
)#";
    EXPECT_EXPR(input, expected);
}

TEST(Parser, BinaryExprWithWhitespace) {
    auto input = " 10 +   2* 3 ";
    auto expected = R"#(
%0 = int(10)
%1 = int(2)
%2 = int(3)
%3 = mul(%1, %2)
%4 = add(%0, %3)
)#";
    EXPECT_EXPR(input, expected);
}

TEST(Parser, BinaryExprWithComments) {
    auto input = "1\n  + 1 // Add one\n+ 10 // Add ten";
    auto expected = R"#(
%0 = int(1)
%1 = int(10)
%2 = add(%0, %1)
)#";
    EXPECT_EXPR(input, expected);
}