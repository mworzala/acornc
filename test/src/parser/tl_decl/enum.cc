#include "../parse_test_check.h"

TEST(Parser, BasicEnum) {
    auto input = "enum foo { }";
    auto expected = R"#(
%0 = enum(foo, cases = _)
)#";
    EXPECT_TL_DECL(input, expected);
}

TEST(Parser, EnumSingleCase) {
    auto input = "enum foo { bar }";
    auto expected = R"#(
%1 = enum(foo, cases = [
  case(bar),
])
)#";
    EXPECT_TL_DECL(input, expected);
}

TEST(Parser, EnumMultiCase) {
    auto input = "enum foo { bar, baz, }";
    auto expected = R"#(
%2 = enum(foo, cases = [
  case(bar),
  case(baz),
])
)#";
    EXPECT_TL_DECL(input, expected);
}
