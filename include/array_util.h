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


typedef struct index_list_s {
    uint32_t size;
    uint32_t capacity;
    uint32_t *data;
} IndexList;

#define self_t IndexList *self

void index_list_init(self_t);
void index_list_free(self_t);
void index_list_add(self_t, uint32_t index);
void index_list_add_multi(self_t, void *data, size_t size);
uint32_t *index_list_get(self_t, uint32_t i);

#define index_list_add_sized(self, data) index_list_add_multi(self, &(data), sizeof(data) / sizeof(uint32_t))

#undef self_t

#endif //ACORNC_ARRAY_UTIL_H
