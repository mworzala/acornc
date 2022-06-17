#include "../parse_test_check.h"

TEST(Parser, BasicIf) {
    auto input = "if true { 1 }";
    auto expected = R"#(
if
  bool "true"
  block
    iret
      int "1"
)#";
    EXPECT_EXPR(input, expected, false);
}

TEST(Parser, BasicIfElse) {
    auto input = "if true { 1 } else { 2 }";
    auto expected = R"#(
if
  bool "true"
  block
    iret
      int "1"
  block
    iret
      int "2"
)#";
    EXPECT_EXPR(input, expected, false);
}

TEST(Parser, BasicIfElseIfElse) {
    auto input = "if true { 1 } else if false { 2 } else { 3 }";
    auto expected = R"#(
if
  bool "true"
  block
    iret
      int "1"
  if
    bool "false"
    block
      iret
        int "2"
    block
      iret
        int "3"
)#";
    EXPECT_EXPR(input, expected, false);
}
