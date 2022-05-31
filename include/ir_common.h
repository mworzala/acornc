#ifndef CONFIG_IR_COMMON_H
#define CONFIG_IR_COMMON_H

#include "common.h"
#include "type.h"

// Incomplete enum representing the known ref values, or any index into an instruction list
// Conversion can be done using the appropriate methods below
typedef enum {
    RefNone,

    RefZero,
    RefOne,

    __REF_LAST,
} Ref;

Ref index_to_ref(uint32_t index);
uint32_t ref_to_index(Ref ref);

char *ref_to_string(Ref ref); //todo safety test

#endif //CONFIG_IR_COMMON_H
