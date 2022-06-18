#include "../parse_test_check.h"

TEST(Hir, LetImplicitType) {
    auto input = R"#(
let foo = 1;
)#";
    auto expected = R"#(
%1 = int(1)
%0 = let(%1) // foo
)#";
    EXPECT_STMT(input, expected);
}

TEST(Hir, LetExplicitType) {
    auto input = R"#(
let foo: i32 = 1;
)#";
    auto expected = R"#(
%2 = int(1)
%3 = as_type(i32, %2)
%0 = let(%3) // foo
)#";
    EXPECT_STMT(input, expected);
}

TEST(Hir, LetExplicitTypeComplexInit) {
    auto input = R"#(
let foo: i32 = 1 + 1;
)#";
    auto expected = R"#(
%3 = int(1)
%4 = int(1)
%2 = add(%3, %4)
%5 = as_type(i32, %2)
%0 = let(%5) // foo
)#";
    EXPECT_STMT(input, expected);
}

//todo this might be valid?
//TEST(Hir, LetExplicitTypeBlockInit) {
//    auto input = R"#(
//let foo: i32 = {
//    1 + 1
//};
//)#";
//    auto expected = R"#(
//%2 = int(1)
//%3 = as_type(i32, %2)
//%0 = let(%3) // foo
//)#";
//    EXPECT_STMT(input, expected);
//}
