#include <gtest/gtest.h>

extern "C" {
#include "hir.h"
}

TEST(Hir, EnsureHirTagStringsPresent) {
    for (int i = 0; i < __HIR_LAST; i++) {
        EXPECT_STRNE(hir_tag_to_string((HirInstTag) i), "<?>");
    }
}
