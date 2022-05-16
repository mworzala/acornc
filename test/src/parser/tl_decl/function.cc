#include "../parse_test_check.h"

TEST(Parser, BasicFnDecl) {
    auto input = R"#(
fn foo() {}
)#";
    auto expected = R"#(
%2 = fn(foo, proto = { params = _, ret = _ }, body = {
  %1 = block(stmts = _)
}
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
}
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
}
)#";
    EXPECT_TL_DECL(input, expected);
}
