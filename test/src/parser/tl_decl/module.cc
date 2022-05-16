#include "../parse_test_check.h"

TEST(Parser, BasicModule) {
    auto input = "";
    auto expected = R"#(
// module
)#";
    EXPECT_MODULE(input, expected);
}

TEST(Parser, SingleDeclModule) {
    auto input = R"#(
struct foo {}
)#";
    auto expected = R"#(
// module

// @1
%0 = struct(foo, fields = _)
)#";
    EXPECT_MODULE(input, expected);
}

TEST(Parser, MultiDeclModule) {
    auto input = R"#(
struct foo {}
struct bar {}
)#";
    auto expected = R"#(
// module

// @1
%0 = struct(foo, fields = _)

// @2
%1 = struct(bar, fields = _)
)#";
    EXPECT_MODULE(input, expected);
}
