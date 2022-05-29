#include "parse_test_check.h"

TEST(Parser, WrongBlockEntryOrdering) {
    auto input = R"#(
{
  let a = bar;
}
)#";
    auto expected = R"#(
%2 = block(stmts = {
  %0 = ref(bar)
  %1 = let(a, type = _, init = %0)
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


TEST(Parser, MissingReturn) {
    auto input = R"#(
fn foo() {
    let a = 42;
    return a;
}
)#";
    auto expected = R"#(
// module

// @1
%6 = fn(foo, proto = { params = _, ret = _ }, body = {
  %5 = block(stmts = {
    // @1
    %1 = int(42)
    %2 = let(a, type = _, init = %1)
    // @2
    %3 = ref(a)
    %4 = ret(%3)
  })
}
)#";
    EXPECT_MODULE(input, expected);
}
