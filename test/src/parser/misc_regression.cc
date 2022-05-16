#include "parse_test_check.h"

TEST(Parser, WrongBlockEntryOrdering) {
    auto input = R"#(
{
  let a = bar;
}
)#";
    auto expected = R"#(
%2 = block(stmts = {
  %1 = let(a, type = _, init = {
    %0 = ref(bar)
  })
})
)#";
    EXPECT_EXPR(input, expected);
}
