#include "../parse_test_check.h"

TEST(Hir, BasicNamedFnDecl) {
    auto input = R"#(
fn foo() {}
)#";
    auto expected = R"#(
foo: %1 = fn(params = [], %2 = block())
)#";
    EXPECT_TL_DECL(input, expected);
}

TEST(Hir, NamedForeignFn) {
    auto input = R"#(
foreign fn foo();
)#";
    auto expected = R"#(
foo: %1 = .foreign fn(params = [])
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
], %6 = block())
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
foo: %1 = fn(params = [], %3 = block({
  %5 = int(1)
  %6 = as_type(i32, %5)
  %4 = ret(%6)
}))
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
foo: %1 = fn(params = [], %2 = block({
  %3 = ret(%0)
}))
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
foo: %1 = fn(params = [], %3 = block({
  %5 = int(1)
  %6 = as_type(i32, %5)
  %4 = ret(%6)
}))
)#";
    EXPECT_TL_DECL(input, expected);
}


TEST(Hir, FnDeclWithImplicitRetRet) {
    auto input = R"#(
fn foo() i32 {
    return 1
}
)#";
    //todo this should be an error, or at least compensated to only add a single return
    auto expected = R"#(
foo: %1 = fn(params = [], %3 = block({
  %6 = int(1)
  %7 = as_type(i32, %6)
  %5 = ret(%7)
  %8 = as_type(i32, %5)
  %4 = ret(%8)
}))
)#";
    EXPECT_TL_DECL(input, expected);
}

//todo test to ensure return as anything but the last statement is invalid
