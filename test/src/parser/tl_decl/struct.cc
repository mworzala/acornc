#include "../parse_test_check.h"

TEST(Parser, BasicStruct) {
    auto input = "struct foo { }";
    auto expected = R"#(
%0 = struct(foo, fields = _)
)#";
    EXPECT_TL_DECL(input, expected);
}

TEST(Parser, StructSingleField) {
    auto input = "struct foo { bar }";
    auto expected = R"#(
%1 = struct(foo, fields = [
  field(bar, type = _),
])
)#";
    EXPECT_TL_DECL(input, expected);
}

TEST(Parser, StructMultiField) {
    auto input = "struct foo { bar; baz; }";
    auto expected = R"#(
%2 = struct(foo, fields = [
  field(bar, type = _),
  field(baz, type = _),
])
)#";
    EXPECT_TL_DECL(input, expected);
}
