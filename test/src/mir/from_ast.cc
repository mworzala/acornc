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
fn foo() i32 {
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
    let a: i32 = 42;
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
fn foo() i32 {
    let a: i32 = 42;
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
fn foo() i32 {
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

TEST(AstToMir, BasicComparison) {
    auto input = R"#(
fn foo() bool {
    return 2 < 3;
}
)#";
    auto expected = R"#(
%1 = constant(i64, 2)
%2 = constant(i64, 3)
%3 = lt(%1, %2)
%4 = ret(%3)
)#";
    EXPECT_MIR(input, expected);
}

TEST(AstToMir, BasicCall) {
    auto input = R"#(
fn main() i32 {
    let a: i32 = 21;
    return add(a, a);
}

fn add(a: i32, b: i32) i32 {
    return a + b;
}
)#";
    auto expected = R"#(
// begin fn main
%1 = alloc(i32)
%2 = constant(i32, 21)
%3 = store(%1, %2)
%4 = fn_ptr(add)
%5 = load(%1)
%6 = load(%1)
%7 = call(%4, args=%5, %6)
%8 = ret(%7)
// end fn main

// begin fn add
%1 = arg(i32, 0)
%2 = arg(i32, 1)
%3 = add(%1, %2)
%4 = ret(%3)
// end fn add
)#";
    EXPECT_MIR_EXT(input, expected);
}

TEST(AstToMir, LetBoolFromCmpLt) {
    auto input = R"#(
fn foo() {
    let a: bool = 1 < 2;
}
)#";
    auto expected = R"#(
%1 = alloc(bool)
%2 = constant(i64, 1)
%3 = constant(i64, 2)
%4 = lt(%2, %3)
%5 = store(%1, %4)
)#";
    EXPECT_MIR(input, expected);
}

TEST(AstToMir, LetPtrType) {
    auto input = R"#(
fn foo() {
    let a: *i32 = 1;
}
)#";
    auto expected = R"#(
%1 = alloc(*i32)
%2 = constant(i64, 1)
%3 = store(%1, %2)
)#";
    EXPECT_MIR(input, expected);
}

TEST(AstToMir, LetPtrPtrType) {
    auto input = R"#(
fn foo() {
    let a: **i32 = 1;
}
)#";
    auto expected = R"#(
%1 = alloc(**i32)
%2 = constant(i64, 1)
%3 = store(%1, %2)
)#";
    EXPECT_MIR(input, expected);
}

TEST(AstToMir, ShouldFailOnIncorrectRetTy) {
    auto input = R"#(
fn foo() i32 {
    return 2 < 3;
}
)#";
    auto expected = R"#(
%1 = constant(i64, 2)
%2 = constant(i64, 3)
%3 = lt(%1, %2)
%4 = ret(%3)
)#";
    ASSERT_DEATH({
        EXPECT_MIR(input, expected);
    }, "Assertion failed: \\(ret_ty == TypeBool\\), function mir_lower_bin_op, file ast_to_mir\\.c");
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