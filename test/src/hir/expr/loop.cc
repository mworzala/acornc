#include "../parse_test_check.h"

TEST(Hir, EmptyLoop) {
    auto input = R"#(
while true { }
)#";
    auto expected = R"#(
%1 = bool(true)
%2 = block()
%0 = loop(%1, %2)
)#";
    EXPECT_EXPR(input, expected);
}
