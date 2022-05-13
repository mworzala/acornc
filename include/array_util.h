#ifndef ACORNC_ARRAY_UTIL_H
#define ACORNC_ARRAY_UTIL_H

#include "common.h"

#define ARRAY_GROW_CAPCITY(capacity) \
    ((capacity) < 8 ? 8 : (capacity) * 2)

#define ARRAY_GROW(type, pointer, new_count) \
    (type *)reallocate(pointer, sizeof(type) * (new_count))

#define ARRAY_FREE(type, pointer) \
    reallocate(pointer, 0)

void *reallocate(void *pointer, size_t new_size);

#endif //ACORNC_ARRAY_UTIL_H
