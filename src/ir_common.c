#include "ir_common.h"

Ref index_to_ref(uint32_t index) {
    return index + __REF_LAST;
}

uint32_t ref_to_index(Ref ref) {
    uint32_t ref_raw = ref;
    if (ref > __REF_LAST) {
        return ref - __REF_LAST;
    } else {
        return 0;
    }
}

char *ref_to_string(Ref ref) {
    switch (ref) {
        case RefNone:
            return "none";
        case RefZero:
            return "zero";
        case RefOne:
            return "one";
        default:
            return "unknown";
    }
}
