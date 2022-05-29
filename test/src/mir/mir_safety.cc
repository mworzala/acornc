#include <gtest/gtest.h>

extern "C" {
#include "mir.h"
}

TEST(Mir, EnsureMirTagStringsPresent) {
    for (int i = 0; i < __MIR_LAST; i++) {
        EXPECT_STRNE(mir_tag_to_string((MirInstTag) i), "<?>");
    }
}
