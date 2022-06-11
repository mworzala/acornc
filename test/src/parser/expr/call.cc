#include "../parse_test_check.h"

TEST(Parser, BasicCall) {
    auto input = "foo()";
    auto expected = R"#(
call
  ref "foo"
)#";
    EXPECT_EXPR(input, expected, false);
}

TEST(Parser, SingleArgCall) {
    auto input = "foo(bar)";
    auto expected = R"#(
call
  ref "foo"
  ref "bar"
)#";
    EXPECT_EXPR(input, expected, false);
}

TEST(Parser, MultiArgCall) {
    auto input = "foo(bar, baz)";
    auto expected = R"#(
call
  ref "foo"
  ref "bar"
  ref "baz"
)#";
    EXPECT_EXPR(input, expected, false);
}

TEST(Parser, CallWithParenInside) {
    auto input = "foo((bar), baz)";
    auto expected = R"#(
call
  ref "foo"
  ref "bar"
  ref "baz"
)#";
    EXPECT_EXPR(input, expected, false);
}

TEST(Parser, CallWithComplexExprInside) {
    auto input = "foo(2 * (bar + 1), baz)";
    auto expected = R"#(
call
  ref "foo"
  binary "*"
    int "2"
    binary "+"
      ref "bar"
      int "1"
  ref "baz"
)#";
    EXPECT_EXPR(input, expected, false);
}

TEST(Parser, CallNested) {
    auto input = "foo(1, baz(2, 3))";
    auto expected = R"#(
call
  ref "foo"
  int "1"
  call
    ref "baz"
    int "2"
    int "3"
)#";
    EXPECT_EXPR(input, expected, false);
}

TEST(Parser, CallWithExprTarget) {
    auto input = "foo.bar()";
    auto expected = R"#(
call
  dot
    ref "foo"
    ref "bar"
)#";
    EXPECT_EXPR(input, expected, false);
}

TEST(Parser, CallComplex1) {
    auto input = "1 + foo.bar()";
    auto expected = R"#(
binary "+"
  int "1"
  call
    dot
      ref "foo"
      ref "bar"
)#";
    EXPECT_EXPR(input, expected, false);
}

TEST(Parser, CallComplex2) {
    auto input = "(1 + foo).bar()";
    auto expected = R"#(
call
  dot
    binary "+"
      int "1"
      ref "foo"
    ref "bar"
)#";
    EXPECT_EXPR(input, expected, false);
}
