#ifndef ACORNC_PARSE_TEST_CHECK_H
#define ACORNC_PARSE_TEST_CHECK_H

#include <gtest/gtest.h>

#define EXPECT_EXPR(expr, expected) \
    EXPECT_PRED2(parse_check_expr, expr, expected + 1)

testing::AssertionResult parse_check_expr(const char *expr, const char *expected);

#endif //ACORNC_PARSE_TEST_CHECK_H
