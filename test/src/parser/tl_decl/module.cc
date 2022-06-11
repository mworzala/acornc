#include "../parse_test_check.h"

TEST(Parser, BasicModule) {
    auto input = "";
    auto expected = R"#(
module
)#";
    EXPECT_MODULE(input, expected);
}

TEST(Parser, SingleDeclModule) {
    auto input = R"#(
struct foo {}
)#";
    auto expected = R"#(
module
  struct "foo"
)#";
    EXPECT_MODULE(input, expected);
}

TEST(Parser, MultiDeclModule) {
    auto input = R"#(
struct foo {}
struct bar {}
)#";
    auto expected = R"#(
module
  struct "foo"
  struct "bar"
)#";
    EXPECT_MODULE(input, expected);
}
