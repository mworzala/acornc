#include <gtest/gtest.h>

extern "C" {
#include "ast.h"
#include "parser.h"
}

TEST(Parser, EnsureAstTagStringsPresent) {
    for (int i = 0; i < __AST_LAST; i++) {
        EXPECT_STRNE(ast_tag_to_string((AstTag) i), "<?>");
    }
}
