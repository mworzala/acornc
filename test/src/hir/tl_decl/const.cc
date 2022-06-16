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
