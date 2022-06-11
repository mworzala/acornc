#include "../parse_test_check.h"

TEST(Parser, BasicWhile) {
    auto input = "while true { }";
    auto expected = R"#(
while
  bool "true"
  block
)#";
    EXPECT_EXPR(input, expected, false);
}
