#include "../parse_test_check.h"

TEST(Parser, BasicEnum) {
    auto input = "enum foo { }";
    auto expected = R"#(
enum "foo"
)#";
    EXPECT_TL_DECL(input, expected);
}

TEST(Parser, EnumSingleCase) {
    auto input = "enum foo { bar }";
    auto expected = R"#(
enum "foo"
  enum_case "bar"
)#";
    EXPECT_TL_DECL(input, expected);
}

TEST(Parser, EnumMultiCase) {
    auto input = "enum foo { bar, baz, }";
    auto expected = R"#(
enum "foo"
  enum_case "bar"
  enum_case "baz"
)#";
    EXPECT_TL_DECL(input, expected);
}
