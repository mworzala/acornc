#include "parse_test_check.h"

TEST(Parser, WrongBlockEntryOrdering) {
    auto input = R"#(
{
  let a = bar;
}
)#";
    auto expected = R"#(
block
  let "a"
    ref "bar"
)#";
    EXPECT_EXPR(input, expected, false);
}


TEST(Parser, EnumOffByOne) {
    auto input = R"#(
struct Person { }
enum Color {
    red,
    green,
    blue,
}
)#";
    auto expected = R"#(
module
  struct "Person"
  enum "Color"
    enum_case "red"
    enum_case "green"
    enum_case "blue"
)#";
    EXPECT_MODULE(input, expected);
}


TEST(Parser, MissingReturn) {
    auto input = R"#(
fn foo() {
    let a = 42;
    return a;
}
)#";
    auto expected = R"#(
module
  named_fn "foo"
    fn_proto
    block
      let "a"
        int "42"
      ret
        ref "a"
)#";
    EXPECT_MODULE(input, expected);
}
