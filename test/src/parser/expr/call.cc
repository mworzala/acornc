#include "../parse_test_check.h"

TEST(Parser, BasicCall) {
    auto input = "foo()";
    auto expected = R"#(
%0 = ref(foo)
%1 = call(%0, args = _)
)#";
    EXPECT_EXPR(input, expected);
}

TEST(Parser, SingleArgCall) {
    auto input = "foo(bar)";
    auto expected = R"#(
%0 = ref(foo)
%1 = ref(bar)
%2 = call(%0, args = [%1])
)#";
    EXPECT_EXPR(input, expected);
}

TEST(Parser, MultiArgCall) {
    auto input = "foo(bar, baz)";
    auto expected = R"#(
%0 = ref(foo)
%1 = ref(bar)
%2 = ref(baz)
%3 = call(%0, args = [%1, %2])
)#";
    EXPECT_EXPR(input, expected);
}

TEST(Parser, CallWithParenInside) {
    auto input = "foo((bar), baz)";
    auto expected = R"#(
%0 = ref(foo)
%1 = ref(bar)
%2 = ref(baz)
%3 = call(%0, args = [%1, %2])
)#";
    EXPECT_EXPR(input, expected);
}

TEST(Parser, CallWithComplexExprInside) {
    auto input = "foo(2 * (bar + 1), baz)";
    auto expected = R"#(
%0 = ref(foo)
%1 = int(2)
%2 = ref(bar)
%3 = int(1)
%4 = add(%2, %3)
%5 = mul(%1, %4)
%6 = ref(baz)
%7 = call(%0, args = [%5, %6])
)#";
    EXPECT_EXPR(input, expected);
}

TEST(Parser, CallNested) {
    auto input = "foo(1, baz(2, 3))";
    auto expected = R"#(
%0 = ref(foo)
%1 = int(1)
%2 = ref(baz)
%3 = int(2)
%4 = int(3)
%5 = call(%2, args = [%3, %4])
%6 = call(%0, args = [%1, %5])
)#";
    EXPECT_EXPR(input, expected);
}

TEST(Parser, CallWithExprTarget) {
    auto input = "foo.bar()";
    auto expected = R"#(
%0 = ref(foo)
%1 = ref(bar)
%2 = dot(%0, %1)
%3 = call(%2, args = _)
)#";
    EXPECT_EXPR(input, expected);
}

TEST(Parser, CallComplex1) {
    auto input = "1 + foo.bar()";
    auto expected = R"#(
%0 = int(1)
%1 = ref(foo)
%2 = ref(bar)
%3 = dot(%1, %2)
%4 = call(%3, args = _)
%5 = add(%0, %4)
)#";
    EXPECT_EXPR(input, expected);
}

TEST(Parser, CallComplex2) {
    auto input = "(1 + foo).bar()";
    auto expected = R"#(
%0 = int(1)
%1 = ref(foo)
%2 = add(%0, %1)
%3 = ref(bar)
%4 = dot(%2, %3)
%5 = call(%4, args = _)
)#";
    EXPECT_EXPR(input, expected);
}
