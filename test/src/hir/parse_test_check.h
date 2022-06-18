#ifndef ACORNC_PARSE_TEST_CHECK_H
#define ACORNC_PARSE_TEST_CHECK_H

#include <gtest/gtest.h>

extern "C" {
#include "parser.h"
#include "parser_internal.h"
#include "ast_lowering.h"
#include "ast_lowering_internal.h"
}

#define EXPECT_MODULE(source, expected) \
    EXPECT_PRED4(lower_check_generic, ast_lower_module, int_module, source, (expected) + 1)

#define EXPECT_TL_DECL(source, expected) \
    EXPECT_PRED4(lower_check_generic, ast_lower_tl_decl, int_top_level_decl, source, (expected) + 1)

#define EXPECT_STMT(source, expected) \
    EXPECT_PRED4(lower_check_generic, ast_lower_stmt, int_stmt, source, (expected) + 1)

#define EXPECT_EXPR(source, expected) \
    EXPECT_PRED4(lower_check_generic, ast_lower_expr, int_expr, source, (expected) + 1)

typedef AstIndex (*ParseFn)(Parser *);
typedef HirIndex (*LowerFn)(AstLowering *, AstIndex);

//todo would be nice to generate a diff between the two.
testing::AssertionResult lower_check_generic(LowerFn lower, ParseFn parse, const char *source, const char *expected);

#endif //ACORNC_PARSE_TEST_CHECK_H
