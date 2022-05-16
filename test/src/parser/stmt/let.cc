#include "../parse_test_check.h"

TEST(Parser, BasicLet) {
    auto input = "let foo";
    auto expected = R"#(
%0 = let(foo, type = _, init = _)
)#";
    EXPECT_STMT(input, expected);
}

TEST(Parser, BasicLetWithInit) {
    auto input = "let foo = 1";
    auto expected = R"#(
%1 = let(foo, type = _, init = {
  %0 = int(1)
})
)#";
    EXPECT_STMT(input, expected);
}

TEST(Parser, LetComplexInit) {
    auto input = "let foo = 1 + 2";
    auto expected = R"#(
%3 = let(foo, type = _, init = {
  %0 = int(1)
  %1 = int(2)
  %2 = add(%0, %1)
})
)#";
    EXPECT_STMT(input, expected);
}

TEST(Parser, LetRequiresSpaceBetweenLetAndIdent) {
    auto input = "letfoo";
    auto expected = R"#(
%0 = ref(letfoo)
)#";
    EXPECT_STMT(input, expected);
}
