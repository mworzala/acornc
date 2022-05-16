#include "../parse_test_check.h"

TEST(Parser, BasicDot) {
    auto input = "foo.bar";
    auto expected = R"#(
%0 = ref(foo)
%1 = ref(bar)
%2 = dot(%0, %1)
)#";
    EXPECT_EXPR(input, expected);
}

TEST(Parser, DotChained) {
    auto input = "foo.bar.baz";
    auto expected = R"#(
%0 = ref(foo)
%1 = ref(bar)
%2 = dot(%0, %1)
%3 = ref(baz)
%4 = dot(%2, %3)
)#";
    EXPECT_EXPR(input, expected);
}

TEST(Parser, DotArithmeticPrecedence) {
    auto input = "foo.bar + baz";
    auto expected = R"#(
%0 = ref(foo)
%1 = ref(bar)
%2 = dot(%0, %1)
%3 = ref(baz)
%4 = add(%2, %3)
)#";
    EXPECT_EXPR(input, expected);
}

TEST(Parser, DotArithmeticPrecedence2) {
    auto input = "foo.bar < foo.baz";
    auto expected = R"#(
%0 = ref(foo)
%1 = ref(bar)
%2 = dot(%0, %1)
%3 = ref(foo)
%4 = ref(baz)
%5 = dot(%3, %4)
%6 = cmp_lt(%2, %5)
)#";
    EXPECT_EXPR(input, expected);
}

TEST(Parser, DotArithmeticPrecedence3) {
    auto input = "foo.bar || foo.baz";
    auto expected = R"#(
%0 = ref(foo)
%1 = ref(bar)
%2 = dot(%0, %1)
%3 = ref(foo)
%4 = ref(baz)
%5 = dot(%3, %4)
%6 = log_or(%2, %5)
)#";
    EXPECT_EXPR(input, expected);
}

TEST(Parser, DotArithmeticPrecedence4) {
    auto input = "(foo + 2).bar + baz";
    auto expected = R"#(
%0 = ref(foo)
%1 = int(2)
%2 = add(%0, %1)
%3 = ref(bar)
%4 = dot(%2, %3)
%5 = ref(baz)
%6 = add(%4, %5)
)#";
    EXPECT_EXPR(input, expected);
}
