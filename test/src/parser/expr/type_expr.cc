#include "../parse_test_check.h"

TEST(Parser, LetWithBasicTypeDecl) {
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