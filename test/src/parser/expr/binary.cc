#include "../parse_test_check.h"

TEST(Parser, SimpleBinary) {
    auto input = "1+2";
    auto expected = R"#(
binary "+"
  int "1"
  int "2"
)#";
    EXPECT_EXPR(input, expected, false);
}

TEST(Parser, SimpleAddMul) {
    auto input = "1+2*3";
    auto expected = R"#(
binary "+"
  int "1"
  binary "*"
    int "2"
    int "3"
)#";
    EXPECT_EXPR(input, expected, false);
}

TEST(Parser, BasicOperators) {
    auto input = "1+2*3/4-1";
    auto expected = R"#(
binary "-"
  binary "+"
    int "1"
    binary "/"
      binary "*"
        int "2"
        int "3"
      int "4"
  int "1"
)#";
    EXPECT_EXPR(input, expected, false);
}

TEST(Parser, BinaryExprWithWhitespace) {
    auto input = " 10 +   2* 3 ";
    auto expected = R"#(
binary "+"
  int "10"
  binary "*"
    int "2"
    int "3"
)#";
    EXPECT_EXPR(input, expected, false);
}

TEST(Parser, BinaryExprWithComments) {
    auto input = "1\n  + 1 // Add one\n+ 10 // Add ten";
    auto expected = R"#(
binary "+"
  binary "+"
    int "1"
    int "1"
  int "10"
)#";
    EXPECT_EXPR(input, expected, false);
}

TEST(Parser, BasicComparison) {
    // Chaining like this is not semantically acceptable,
    // however it is syntactically just binary expressions.
    auto input = "1 < 2 <= 3 > 4 >= 5 == 6 != 7";
    auto expected = R"#(
binary "!="
  binary "=="
    binary ">="
      binary ">"
        binary "<="
          binary "<"
            int "1"
            int "2"
          int "3"
        int "4"
      int "5"
    int "6"
  int "7"
)#";
    EXPECT_EXPR(input, expected, false);
}

TEST(Parser, ComparisonHigherThanArithmetic) {
    // Chaining like this is not semantically acceptable,
    // however it is syntactically just binary expressions.
    auto input = "1 + 2 < 3 + 4";
    auto expected = R"#(
binary "<"
  binary "+"
    int "1"
    int "2"
  binary "+"
    int "3"
    int "4"
)#";
    EXPECT_EXPR(input, expected, false);
}

TEST(Parser, BasicLogical) {
    // Chaining like this is not semantically acceptable,
    // however it is syntactically just binary expressions.
    auto input = "true && false || true";
    auto expected = R"#(
binary "||"
  binary "&&"
    bool "true"
    bool "false"
  bool "true"
)#";
    EXPECT_EXPR(input, expected, false);
}

TEST(Parser, LogicalHigherThanArithmetic) {
    // Chaining like this is not semantically acceptable,
    // however it is syntactically just binary expressions.
    auto input = "1 + 2 || 3 + 4";
    auto expected = R"#(
binary "||"
  binary "+"
    int "1"
    int "2"
  binary "+"
    int "3"
    int "4"
)#";
    EXPECT_EXPR(input, expected, false);
}

TEST(Parser, LogicalHigherThanComparison) {
    // Chaining like this is not semantically acceptable,
    // however it is syntactically just binary expressions.
    auto input = "1 < 2 && 3 > 4";
    auto expected = R"#(
binary "&&"
  binary "<"
    int "1"
    int "2"
  binary ">"
    int "3"
    int "4"
)#";
    EXPECT_EXPR(input, expected, false);
}

TEST(Parser, ComplexBinaryExpr) {
    // Chaining like this is not semantically acceptable,
    // however it is syntactically just binary expressions.
    auto input = "1 + 2 < 3 + 4 && 5 + 6 > 7 + 8";
    auto expected = R"#(
binary "&&"
  binary "<"
    binary "+"
      int "1"
      int "2"
    binary "+"
      int "3"
      int "4"
  binary ">"
    binary "+"
      int "5"
      int "6"
    binary "+"
      int "7"
      int "8"
)#";
    EXPECT_EXPR(input, expected, false);
}

// Error cases

//TEST(Parser, BinaryMissingRHS) {
//    auto input = "1+";
//    auto expected = R"#(
//%0 = int(1)
//%1 = int(2)
//%2 = int(3)
//%3 = mul(%1, %2)
//%4 = add(%0, %3)
//)#";
//    EXPECT_EXPR(input, expected, false);
//}
