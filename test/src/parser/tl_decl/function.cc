#include "../parse_test_check.h"

TEST(Parser, BasicFnDecl) {
    auto input = R"#(
fn foo() {}
)#";
    auto expected = R"#(
%2 = fn(foo, proto = { params = _, ret = _ }, body = {
  %1 = block(stmts = _)
})
)#";
    EXPECT_TL_DECL(input, expected);
}

TEST(Parser, FnDeclSingleParam) {
    auto input = R"#(
fn foo(bar) {}
)#";
    auto expected = R"#(
%3 = fn(foo, proto = { params = [
  param(bar, type = _),
], ret = _ }, body = {
  %2 = block(stmts = _)
})
)#";
    EXPECT_TL_DECL(input, expected);
}

TEST(Parser, FnDeclMultiParam) {
    auto input = R"#(
fn foo(bar, baz) {}
)#";
    auto expected = R"#(
%4 = fn(foo, proto = { params = [
  param(bar, type = _),
  param(baz, type = _),
], ret = _ }, body = {
  %3 = block(stmts = _)
})
)#";
    EXPECT_TL_DECL(input, expected);
}

TEST(Parser, FnDeclMultiParamWithTypes) {
    auto input = R"#(
fn foo(bar: i32, baz: i32) {}
)#";
    auto expected = R"#(
%6 = fn(foo, proto = { params = [
  param(bar, type = type(i32)),
  param(baz, type = type(i32)),
], ret = _ }, body = {
  %5 = block(stmts = _)
})
)#";
    EXPECT_TL_DECL(input, expected);
}

TEST(Parser, FnDeclWithReturnType) {
    auto input = R"#(
fn foo() i32 {}
)#";
    auto expected = R"#(
%3 = fn(foo, proto = { params = _, ret = type(i32) }, body = {
  %2 = block(stmts = _)
})
)#";
    EXPECT_TL_DECL(input, expected);
}

TEST(Parser, ForeignFnDecl) {
    auto input = R"#(
foreign fn puts(s: *i8) i32;
)#";
    auto expected = R"#(
%5 = fn(puts, proto = { foreign, params = [
  param(s, type = type(*, inner = type(i8))),
], ret = type(i32) }, body = _)
)#";
    EXPECT_TL_DECL(input, expected);
}

