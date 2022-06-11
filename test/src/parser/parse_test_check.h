#ifndef ACORNC_PARSE_TEST_CHECK_H
#define ACORNC_PARSE_TEST_CHECK_H

#include <gtest/gtest.h>

extern "C" {
#include "parser.h"
#include "parser_internal.h"
}

#define EXPECT_EXPR(expr, expected, locs) \
    EXPECT_PRED4(parse_check_generic, int_expr, expr, (expected) + 1, locs)

#define EXPECT_STMT(expr, expected) \
    EXPECT_PRED4(parse_check_generic, int_stmt, expr, (expected) + 1, false)

#define EXPECT_TL_DECL(expr, expected) \
    EXPECT_PRED4(parse_check_generic, int_top_level_decl, expr, (expected) + 1, false)

#define EXPECT_MODULE(expr, expected) \
    EXPECT_PRED4(parse_check_generic, int_module, expr, (expected) + 1, false)


typedef AstIndex (*ParseFn)(Parser *);

//todo would be nice to generate a diff between the two.
testing::AssertionResult parse_check_generic(ParseFn parse, const char *expr, const char *expected, bool print_locs);

#endif //ACORNC_PARSE_TEST_CHECK_H
