#include "parse_test_check.h"

TEST(AstToMir, EmptyTest) {
    auto input = R"#(
fn foo() {

}
)#";
    auto expected = R"#(

)#";
    EXPECT_MIR(input, expected);
}

TEST(AstToMir, SimpleReturn) {
    auto input = R"#(
fn foo() {
    return 42;
}
)#";
    auto expected = R"#(
%1 = constant(i32, 42)
%2 = ret(%1)
)#";
    EXPECT_MIR(input, expected);
}

TEST(AstToMir, LetStmt) {
    auto input = R"#(
fn foo() {
    let a = 42;
}
)#";
    auto expected = R"#(
%1 = alloc(i32)
%2 = constant(i32, 42)
%3 = store(%1, %2)
)#";
    EXPECT_MIR(input, expected);
}

TEST(AstToMir, BasicReference) {
    auto input = R"#(
fn foo() {
    let a = 42;
    return a;
}
)#";
    auto expected = R"#(
%1 = alloc(i32)
%2 = constant(i32, 42)
%3 = store(%1, %2)
%4 = load(%1)
%5 = ret(%4)
)#";
    EXPECT_MIR(input, expected);
}

//TEST(AstToMir, SingleIntReplacedWithRef) {
//    auto input = R"#(
//fn foo() {
//    1;
//}
//)#";
//    auto expected = R"#(
//%1 = @Ref.one
//)#";
//    EXPECT_MIR(input, expected);
//}