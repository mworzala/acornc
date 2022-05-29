#ifndef ACORNC_PARSE_TEST_CHECK_H
#define ACORNC_PARSE_TEST_CHECK_H

#include <gtest/gtest.h>

extern "C" {
#include "mir.h"
}

#define EXPECT_MIR(expr, expected) \
    EXPECT_PRED2(parse_check_mir, expr, (expected) + 1)


testing::AssertionResult parse_check_mir(const char *expr, const char *expected);

#endif //ACORNC_PARSE_TEST_CHECK_H
