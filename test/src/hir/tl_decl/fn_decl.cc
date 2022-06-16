#include "../parse_test_check.h"

TEST(Hir, BasicNamedFnDecl) {
    auto input = R"#(
fn foo() {}
)#";
    auto expected = R"#(
foo: %1 = fn(params = [], %3 = block())
)#";
    EXPECT_TL_DECL(input, expected);
}

TEST(Hir, NamedForeignFn) {
    auto input = R"#(
foreign fn foo() {}
)#";
    auto expected = R"#(
foo: %1 = .foreign fn(params = [], %3 = block())
)#";
    EXPECT_TL_DECL(input, expected);
}

TEST(Hir, BasicFnWithParams) {
    auto input = R"#(
fn foo(x: i32, y: i32) {}
)#";
    auto expected = R"#(
foo: %1 = fn(params = [
  x: i32
  y: i32
], %3 = block())
)#";
    EXPECT_TL_DECL(input, expected);
}

TEST(Hir, FnDeclWithExplicitRet) {
    auto input = R"#(
fn foo() i32 {
    return 1;
}
)#";
    auto expected = R"#(
foo: %1 = fn(block = {
  %2 = int(1)
  %3 = as_type(i32, %2)
  %4 = ret(%3)
})
)#";
    EXPECT_TL_DECL(input, expected);
}

TEST(Hir, FnDeclWithExplicitRetEmpty) {
    auto input = R"#(
fn foo() {
    return;
}
)#";
    auto expected = R"#(
foo: %1 = fn(block = {
  %2 = int(1)
  %3 = as_type(i32, %2)
  %4 = ret(%3)
})
)#";
    EXPECT_TL_DECL(input, expected);
}

TEST(Hir, FnDeclWithImplicitRet) {
    auto input = R"#(
fn foo() i32 {
    1
}
)#";
    auto expected = R"#(
foo: %1 = fn(block = {
  %2 = int(1)
  %3 = as_type(i32, %2)
  %4 = ret(%3)
})
)#";
    EXPECT_TL_DECL(input, expected);
}

//todo test to ensure return as anything but the last statement is invalid
