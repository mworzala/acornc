#include "../parse_test_check.h"

TEST(Parser, BasicTypeExpression) {
    auto input = R"#(
let a: i32 = 1;
)#";
    auto expected = R"#(
%0 = type(i32)
%1 = int(1)
%2 = let(a, type = %0, init = %1)
)#";
    EXPECT_STMT(input, expected);
}

TEST(Parser, PtrTypeExpression) {
    auto input = R"#(
let a: *i32 = 1;
)#";
    auto expected = R"#(
%0 = type(i32)
%1 = type(*, inner = %0)
%2 = int(1)
%3 = let(a, type = %1, init = %2)
)#";
    EXPECT_STMT(input, expected);
}