#include "../parse_test_check.h"

TEST(Parser, BasicFnDecl) {
    auto input = R"#(
fn foo() {}
)#";
    auto expected = R"#(
named_fn "foo"
  fn_proto
  block
)#";
    EXPECT_TL_DECL(input, expected);
}

TEST(Parser, FnDeclSingleParam) {
    auto input = R"#(
fn foo(bar) {}
)#";
    auto expected = R"#(
named_fn "foo"
  fn_proto
    fn_param "bar"
  block
)#";
    EXPECT_TL_DECL(input, expected);
}

TEST(Parser, FnDeclMultiParam) {
    auto input = R"#(
fn foo(bar, baz) {}
)#";
    auto expected = R"#(
named_fn "foo"
  fn_proto
    fn_param "bar"
    fn_param "baz"
  block
)#";
    EXPECT_TL_DECL(input, expected);
}

TEST(Parser, FnDeclMultiParamWithTypes) {
    auto input = R"#(
fn foo(bar: i32, baz: i32) {}
)#";
    auto expected = R"#(
named_fn "foo"
  fn_proto
    fn_param "bar"
      type "i32"
    fn_param "baz"
      type "i32"
  block
)#";
    EXPECT_TL_DECL(input, expected);
}

TEST(Parser, FnDeclWithReturnType) {
    auto input = R"#(
fn foo() i32 {}
)#";
    auto expected = R"#(
named_fn "foo"
  fn_proto
    type "i32"
  block
)#";
    EXPECT_TL_DECL(input, expected);
}

TEST(Parser, ForeignFnDecl) {
    auto input = R"#(
foreign fn puts(s: *i8) i32;
)#";
    auto expected = R"#(
named_fn "puts"
  fn_proto
    flags foreign
    fn_param "s"
      type "*"
        type "i8"
    type "i32"
)#";
    EXPECT_TL_DECL(input, expected);
}

