#include "../parse_test_check.h"

TEST(Parser, BasicDot) {
    auto input = "foo.bar";
    auto expected = R"#(
dot
  ref "foo"
  ref "bar"
)#";
    EXPECT_EXPR(input, expected, false);
}

TEST(Parser, DotChained) {
    auto input = "foo.bar.baz";
    auto expected = R"#(
dot
  dot
    ref "foo"
    ref "bar"
  ref "baz"
)#";
    EXPECT_EXPR(input, expected, false);
}

TEST(Parser, DotArithmeticPrecedence) {
    auto input = "foo.bar + baz";
    auto expected = R"#(
binary "+"
  dot
    ref "foo"
    ref "bar"
  ref "baz"
)#";
    EXPECT_EXPR(input, expected, false);
}

TEST(Parser, DotArithmeticPrecedence2) {
    auto input = "foo.bar < foo.baz";
    auto expected = R"#(
binary "<"
  dot
    ref "foo"
    ref "bar"
  dot
    ref "foo"
    ref "baz"
)#";
    EXPECT_EXPR(input, expected, false);
}

TEST(Parser, DotArithmeticPrecedence3) {
    auto input = "foo.bar || foo.baz";
    auto expected = R"#(
binary "||"
  dot
    ref "foo"
    ref "bar"
  dot
    ref "foo"
    ref "baz"
)#";
    EXPECT_EXPR(input, expected, false);
}

TEST(Parser, DotArithmeticPrecedence4) {
    auto input = "(foo + 2).bar + baz";
    auto expected = R"#(
binary "+"
  dot
    binary "+"
      ref "foo"
      int "2"
    ref "bar"
  ref "baz"
)#";
    EXPECT_EXPR(input, expected, false);
}
