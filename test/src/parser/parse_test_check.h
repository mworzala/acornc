#ifndef ACORNC_PARSE_TEST_CHECK_H
#define ACORNC_PARSE_TEST_CHECK_H

#include <gtest/gtest.h>

extern "C" {
#include "parser.h"
#include "parser_internal.h"
}

#define EXPECT_EXPR(expr, expected) \
    EXPECT_PRED3(parse_check_generic, int_expr, expr, (expected) + 1)

#define EXPECT_STMT(expr, expected) \
    EXPECT_PRED3(parse_check_generic, int_stmt, expr, (expected) + 1)


typedef AstIndex (*ParseFn)(Parser *);

testing::AssertionResult parse_check_generic(ParseFn parse, const char *expr, const char *expected);

#endif //ACORNC_PARSE_TEST_CHECK_H
