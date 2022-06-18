#include "../parse_test_check.h"

TEST(Hir, RefLocalLet) {
    auto input = R"#(
{
    let foo = 1;
    foo;
}
)#";
    auto expected = R"#(
%0 = block({
  %2 = int(1)
  %1 = let(%2) // foo

  %3 = ref(%1)
})
)#";
    EXPECT_EXPR(input, expected);
}

TEST(Hir, RefParam) {
    auto input = R"#(
fn foo(bar: i32) {
    bar;
}
)#";
    auto expected = R"#(
foo: %1 = fn(params = [
  bar: %2 = param(i32)
], %4 = block({
  %5 = ref(%2)
}))
)#";
    EXPECT_TL_DECL(input, expected);
}

//todo test for nonexistent ref
