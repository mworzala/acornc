#include "parse_test_check.h"

TEST(Parser, BasicArithmetic) {
    auto input = "1+2";
    auto expected = R"#(
%0 = int(1)
%1 = int(2)
%2 = add(%0, %1)
)#";
    EXPECT_EXPR(input, expected);
}