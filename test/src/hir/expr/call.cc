#include "../parse_test_check.h"

TEST(Hir, NoArgsCallName) {
    //todo the block and foo definition is required, otherwise `foo` will not be resolved. Need a better solution
    auto input = R"#(
{
    let foo = 0;
    foo();
}
)#";
    auto expected = R"#(
%0 = block({
  %2 = int(0)
  %1 = let(%2) // foo

  %4 = ref(%1)
  %3 = call(%4, args = [])
})
)#";
    EXPECT_EXPR(input, expected);
}

TEST(Hir, SingleArgCallName) {
    //todo the block and foo definition is required, otherwise `foo` will not be resolved. Need a better solution
    auto input = R"#(
{
    let foo = 0;
    foo(1);
}
)#";
    auto expected = R"#(
%0 = block({
  %2 = int(0)
  %1 = let(%2) // foo

  %4 = ref(%1)
  %5 = int(1)
  %3 = call(%4, args = [%5])
})
)#";
    EXPECT_EXPR(input, expected);
}

TEST(Hir, MultiArgCallName) {
    //todo the block and foo definition is required, otherwise `foo` will not be resolved. Need a better solution
    auto input = R"#(
{
    let foo = 0;
    foo(1, 2, 3);
}
)#";
    auto expected = R"#(
%0 = block({
  %2 = int(0)
  %1 = let(%2) // foo

  %4 = ref(%1)
  %5 = int(1)
  %6 = int(2)
  %7 = int(3)
  %3 = call(%4, args = [%5, %6, %7])
})
)#";
    EXPECT_EXPR(input, expected);
}
