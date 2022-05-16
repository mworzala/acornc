#include "parse_test_check.h"

TEST(Parser, WrongBlockEntryOrdering) {
    auto input = R"#(
{
  let a = bar;
}
)#";
    auto expected = R"#(
%2 = block(stmts = {
  %1 = let(a, type = _, init = {
    %0 = ref(bar)
  })
})
)#";
    EXPECT_EXPR(input, expected);
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
// module

// @1
%0 = struct(Person, fields = _)

// @2
%4 = enum(Color, cases = [
  case(red),
  case(green),
  case(blue),
])
)#";
    EXPECT_MODULE(input, expected);
}
