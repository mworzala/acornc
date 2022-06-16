#include "../parse_test_check.h"

TEST(Parser, BasicConst) {
    auto input = "const foo = 1";
    auto expected = R"#(
const "foo"
  int "1"
)#";
    EXPECT_TL_DECL(input, expected);
}

TEST(Parser, BasicConstWithTypeExpr) {
    auto input = "const foo: i32 = 1";
    auto expected = R"#(
const "foo"
  type "i32"
  int "1"
)#";
    EXPECT_TL_DECL(input, expected);
}
