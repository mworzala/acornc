#include "../parse_test_check.h"

TEST(Parser, BasicWhile) {
    auto input = "while true { }";
    auto expected = R"#(
%0 = bool(true)
%1 = block(stmts = _)
%2 = while(%0, body = %1)
)#";
    EXPECT_EXPR(input, expected);
}
