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
%1 = type(*, inner = type(i32))
%2 = int(1)
%3 = let(a, type = %1, init = %2)
)#";
    EXPECT_STMT(input, expected);
}

TEST(Parser, PtrPtrTypeExpression) {
    auto input = R"#(
let a: **i32 = 1;
)#";
    auto expected = R"#(
%2 = type(*, inner = type(*, inner = type(i32)))
%3 = int(1)
%4 = let(a, type = %2, init = %3)
)#";
    EXPECT_STMT(input, expected);
}