#include "../parse_test_check.h"

TEST(Parser, BasicLet) {
    auto input = "let foo";
    auto expected = R"#(
let "foo"
)#";
    EXPECT_STMT(input, expected);
}

TEST(Parser, BasicLetWithInit) {
    auto input = "let foo = 1";
    auto expected = R"#(
let "foo"
  int "1"
)#";
    EXPECT_STMT(input, expected);
}

TEST(Parser, LetComplexInit) {
    auto input = "let foo = 1 + 2";
    auto expected = R"#(
let "foo"
  binary "+"
    int "1"
    int "2"
)#";
    EXPECT_STMT(input, expected);
}

TEST(Parser, LetRequiresSpaceBetweenLetAndIdent) {
    auto input = "letfoo";
    auto expected = R"#(
ref "letfoo"
)#";
    EXPECT_STMT(input, expected);
}

TEST(Parser, LetWithBasicTypeAnnotation) {
    auto input = "let foo: i32 = 1";
    auto expected = R"#(
let "foo"
  type "i32"
  int "1"
)#";
    EXPECT_STMT(input, expected);
}

TEST(Parser, LetWithExplicitTypeInference) {
    auto input = "let foo: _ = 1";
    auto expected = R"#(
let "foo"
  type "_"
  int "1"
)#";
    EXPECT_STMT(input, expected);
}

TEST(Parser, LetWithNoInitializer) {
    auto input = "let foo =";
    //todo eof not the correct error here
    auto expected = R"#(
let "foo"
  error
ERR : unexpected end of file @ 9
)#";
    EXPECT_STMT(input, expected);
}

TEST(Parser, LetMissingInitializerParseBelow) {
    auto input = "{ let foo =;\nlet bar = 1; }";
    //todo incorrect error
    auto expected = R"#(
block
  let "foo"
    error
  let "bar"
    int "1"
ERR : expected expression, found ... @ 11
)#";
    EXPECT_STMT(input, expected);
}
