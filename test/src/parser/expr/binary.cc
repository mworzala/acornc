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
%1 = int(1)
%2 = add(%0, %1)
%3 = int(10)
%4 = add(%2, %3)
)#";
    EXPECT_EXPR(input, expected);
}

TEST(Parser, BasicComparison) {
    // Chaining like this is not semantically acceptable,
    // however it is syntactically just binary expressions.
    auto input = "1 < 2 <= 3 > 4 >= 5 == 6 != 7";
    auto expected = R"#(
%0 = int(1)
%1 = int(2)
%2 = cmp_lt(%0, %1)
%3 = int(3)
%4 = cmp_le(%2, %3)
%5 = int(4)
%6 = cmp_gt(%4, %5)
%7 = int(5)
%8 = cmp_ge(%6, %7)
%9 = int(6)
%10 = cmp_eq(%8, %9)
%11 = int(7)
%12 = cmp_ne(%10, %11)
)#";
    EXPECT_EXPR(input, expected);
}

TEST(Parser, ComparisonHigherThanArithmetic) {
    // Chaining like this is not semantically acceptable,
    // however it is syntactically just binary expressions.
    auto input = "1 + 2 < 3 + 4";
    auto expected = R"#(
%0 = int(1)
%1 = int(2)
%2 = add(%0, %1)
%3 = int(3)
%4 = int(4)
%5 = add(%3, %4)
%6 = cmp_lt(%2, %5)
)#";
    EXPECT_EXPR(input, expected);
}

TEST(Parser, BasicLogical) {
    // Chaining like this is not semantically acceptable,
    // however it is syntactically just binary expressions.
    auto input = "true && false || true";
    auto expected = R"#(
%0 = bool(true)
%1 = bool(false)
%2 = log_and(%0, %1)
%3 = bool(true)
%4 = log_or(%2, %3)
)#";
    EXPECT_EXPR(input, expected);
}

TEST(Parser, LogicalHigherThanArithmetic) {
    // Chaining like this is not semantically acceptable,
    // however it is syntactically just binary expressions.
    auto input = "1 + 2 || 3 + 4";
    auto expected = R"#(
%0 = int(1)
%1 = int(2)
%2 = add(%0, %1)
%3 = int(3)
%4 = int(4)
%5 = add(%3, %4)
%6 = log_or(%2, %5)
)#";
    EXPECT_EXPR(input, expected);
}

TEST(Parser, LogicalHigherThanComparison) {
    // Chaining like this is not semantically acceptable,
    // however it is syntactically just binary expressions.
    auto input = "1 < 2 && 3 > 4";
    auto expected = R"#(
%0 = int(1)
%1 = int(2)
%2 = cmp_lt(%0, %1)
%3 = int(3)
%4 = int(4)
%5 = cmp_gt(%3, %4)
%6 = log_and(%2, %5)
)#";
    EXPECT_EXPR(input, expected);
}

TEST(Parser, ComplexBinaryExpr) {
    // Chaining like this is not semantically acceptable,
    // however it is syntactically just binary expressions.
    auto input = "1 + 2 < 3 + 4 && 5 + 6 > 7 + 8";
    auto expected = R"#(
%0 = int(1)
%1 = int(2)
%2 = add(%0, %1)
%3 = int(3)
%4 = int(4)
%5 = add(%3, %4)
%6 = cmp_lt(%2, %5)
%7 = int(5)
%8 = int(6)
%9 = add(%7, %8)
%10 = int(7)
%11 = int(8)
%12 = add(%10, %11)
%13 = cmp_gt(%9, %12)
%14 = log_and(%6, %13)
)#";
    EXPECT_EXPR(input, expected);
}

// Error cases

TEST(Parser, BinaryMissingRHS) {
    auto input = "1+";
    auto expected = R"#(
%0 = int(1)
%1 = int(2)
%2 = int(3)
%3 = mul(%1, %2)
%4 = add(%0, %3)
)#";
    EXPECT_EXPR(input, expected);
}
