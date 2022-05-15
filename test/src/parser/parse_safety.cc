#include <gtest/gtest.h>

extern "C" {
#include "ast.h"
#include "parser.h"
#include "debug/ast_debug.h"
}

TEST(Parser, EnsureAstTagStringsPresent) {
    for (int i = 0; i < __AST_LAST; i++) {
        EXPECT_STRNE(ast_tag_to_string((AstTag) i), "<?>");
    }
}

TEST(Parser, EnsureAllDebugFnsPresent) {
    for (auto &ast_debug_fn : ast_debug_fns) {
        EXPECT_NE(ast_debug_fn, (void*) nullptr);
    }
}
