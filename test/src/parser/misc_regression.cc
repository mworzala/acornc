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
%1 = struct(Person, fields = _)

// @2
%5 = enum(Color, cases = [
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
%7 = fn(foo, proto = { params = _, ret = _ }, body = {
  %6 = block(stmts = {
    // @1
    %2 = int(42)
    %3 = let(a, type = _, init = %2)
    // @2
    %4 = ref(a)
    %5 = ret(%4)
  })
}
)#";
    EXPECT_MODULE(input, expected);
}
