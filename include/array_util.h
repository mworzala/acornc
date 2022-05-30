#ifndef ACORNC_ARRAY_UTIL_H
#define ACORNC_ARRAY_UTIL_H

#include "common.h"


#define ARRAY_GROW_CAPCITY(capacity) \
    ((capacity) < 8 ? 8 : (capacity) * 2)

#define ARRAY_GROW(type, pointer, new_count) \
    (type *)reallocate(pointer, sizeof(type) * (new_count))

#define ARRAY_GROW2(type, pointer, old_count, new_count) \
    (type *)reallocate2(pointer, sizeof(type) * (old_count), sizeof(type) * (new_count))

#define ARRAY_FREE(type, pointer) \
    reallocate(pointer, 0)

void *reallocate(void *pointer, size_t new_size);

// SECTION: Index List
// Stores a list of indices in order.

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
bool index_list_contains(self_t, uint32_t index);

#define index_list_add_sized(self, data) index_list_add_multi(self, &(data), sizeof(data) / sizeof(uint32_t))
#define index_list_get_sized(self, Type, start_index) *((Type*) index_list_get(start_index);

#undef self_t

// SECTION: Index Map
// Stores a mapping between two sets of indices.

typedef struct index_map_s {
    uint32_t capacity;
    uint32_t *data;
} IndexMap;

#define self_t IndexMap *self

void index_map_init(self_t);
void index_map_free(self_t);
void index_map_put(self_t, uint32_t key, uint32_t value);
uint32_t *index_map_get(self_t, uint32_t key);

#undef self_t

// SECTION: Index-Pointer Map
// Stores a mapping between two sets of indices.

typedef struct index_ptr_map_s {
    uint32_t capacity;
    size_t *data;
} IndexPtrMap;

#define self_t IndexPtrMap *self

void index_ptr_map_init(self_t);
void index_ptr_map_free(self_t);
void index_ptr_map_put(self_t, uint32_t key, size_t value);
size_t *index_ptr_map_get(self_t, uint32_t key);

#undef self_t

#endif //ACORNC_ARRAY_UTIL_H
