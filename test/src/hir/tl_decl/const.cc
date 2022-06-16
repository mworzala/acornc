#include "../parse_test_check.h"

TEST(Hir, BasicConst) {
    auto input = "const foo = 1";
    auto expected = R"#(
foo: %0 = todo({
  %1 = int(1)
})
)#";
    EXPECT_TL_DECL(input, expected);
}

TEST(Hir, ConstWithTypeAnnotation) {
    auto input = "const foo: i32 = 1";
    auto expected = R"#(
foo: %0 = todo({
  %1 = int(1)
  %2 = as_type(i32, %1)
})
)#";
    EXPECT_TL_DECL(input, expected);
}

TEST(Hir, ConstWithPtrTypeAnnotation) {
    auto input = "const foo: *i32 = 1";
    auto expected = R"#(
foo: %0 = todo({
  %1 = int(1)
  %2 = as_type(*i32, %1)
})
)#";
    EXPECT_TL_DECL(input, expected);
}

TEST(Hir, ConstWithString) {
    auto input = "const foo = \"Hello, world!\"";
    auto expected = R"#(
foo: %0 = todo({
  %1 = str("Hello, world!")
})
)#";
    EXPECT_TL_DECL(input, expected);
}
