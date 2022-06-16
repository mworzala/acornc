#ifndef CONFIG_MIR_DEBUG_H
#define CONFIG_MIR_DEBUG_H

#include "common.h"
#include "hir.h"

// Unsafe as usual
char *hir_debug_print(Hir *hir, HirIndex root);

#endif //CONFIG_MIR_DEBUG_H
