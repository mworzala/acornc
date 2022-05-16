#include "../parse_test_check.h"

TEST(Parser, BasicIf) {
    auto input = "if true { 1 }";
    auto expected = R"#(
%0 = bool(true)
%2 = block(stmts = {
  %1 = int(1)
})
%3 = if(%0, then = %2, else = _)
)#";
    EXPECT_EXPR(input, expected);
}

TEST(Parser, BasicIfElse) {
    auto input = "if true { 1 } else { 2 }";
    auto expected = R"#(
%0 = bool(true)
%2 = block(stmts = {
  %1 = int(1)
})
%4 = block(stmts = {
  %3 = int(2)
})
%5 = if(%0, then = %2, else = %4)
)#";
    EXPECT_EXPR(input, expected);
}

TEST(Parser, BasicIfElseIfElse) {
    auto input = "if true { 1 } else if false { 2 } else { 3 }";
    auto expected = R"#(
%0 = bool(true)
%2 = block(stmts = {
  %1 = int(1)
})
%3 = bool(false)
%5 = block(stmts = {
  %4 = int(2)
})
%7 = block(stmts = {
  %6 = int(3)
})
%8 = if(%3, then = %5, else = %7)
%9 = if(%0, then = %2, else = %8)
)#";
    EXPECT_EXPR(input, expected);
}
