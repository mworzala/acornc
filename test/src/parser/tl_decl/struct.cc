#include "../parse_test_check.h"

TEST(Parser, BasicStruct) {
    auto input = "struct foo { }";
    auto expected = R"#(
struct "foo"
)#";
    EXPECT_TL_DECL(input, expected);
}

TEST(Parser, StructSingleField) {
    auto input = "struct foo { bar }";
    auto expected = R"#(
struct "foo"
  field "bar"
)#";
    EXPECT_TL_DECL(input, expected);
}

TEST(Parser, StructMultiField) {
    auto input = "struct foo { bar; baz; }";
    auto expected = R"#(
struct "foo"
  field "bar"
  field "baz"
)#";
    EXPECT_TL_DECL(input, expected);
}

//todo fields with types. Defaults.
