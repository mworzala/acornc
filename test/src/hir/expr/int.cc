#include "../parse_test_check.h"

TEST(Hir, SmallInt) {
    auto input = "1";
    auto expected = R"#(
%0 = int(1)
)#";
    EXPECT_EXPR(input, expected);
}
