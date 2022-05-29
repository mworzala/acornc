#include "parse_test_check.h"

TEST(Parser, EmptyTest) {
    auto input = R"#(
fn foo() {

}
)#";
    auto expected = R"#(

)#";
    EXPECT_MIR(input, expected);
}