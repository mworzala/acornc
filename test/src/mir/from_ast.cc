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

TEST(AstToMir, BasicAdd) {
    auto input = R"#(
fn foo() {
    return 2 + 3;
}
)#";
    auto expected = R"#(
%1 = constant(i32, 2)
%2 = constant(i32, 3)
%3 = add(%1, %2)
%4 = ret(%3)
)#";
    EXPECT_MIR(input, expected);
}

TEST(AstToMir, BasicCall) {
    auto input = R"#(
fn main() {
    let a = 21;
    return add(a, a);
}

fn add(a, b) {
    return a + b;
}
)#";
    auto expected = R"#(
// begin fn main
%1 = alloc(i32)
%2 = constant(i32, 21)
%3 = store(%1, %2)
%4 = ref(a)
%5 = ref(a)
%6 = call(add, args = [%4, %5])
%7 = ret(%6)
// end fn main

// begin fn add
%8 = param(0)
%9 = param(1)
%10 = add(%8, %9)
%11 = ret(%10)
//end fn add
)#";
    EXPECT_MIR_EXT(input, expected);
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