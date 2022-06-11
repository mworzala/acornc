#include "../parse_test_check.h"

TEST(Parser, BasicTypeExpression) {
    auto input = R"#(
let a: i32 = 1;
)#";
    auto expected = R"#(
let "a"
  type "i32"
  int "1"
)#";
    EXPECT_STMT(input, expected);
}

TEST(Parser, PtrTypeExpression) {
    auto input = R"#(
let a: *i32 = 1;
)#";
    auto expected = R"#(
let "a"
  type "*"
    type "i32"
  int "1"
)#";
    EXPECT_STMT(input, expected);
}

TEST(Parser, PtrPtrTypeExpression) {
    auto input = R"#(
let a: **i32 = 1;
)#";
    auto expected = R"#(
let "a"
  type "*"
    type "*"
      type "i32"
  int "1"
)#";
    EXPECT_STMT(input, expected);
}