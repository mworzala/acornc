#include "array_util.h"

#include <stdlib.h>
#include <assert.h>
#include <string.h>

void *reallocate(void *pointer, size_t new_size) {
    if (new_size == 0) {
        free(pointer);
        return NULL;
    }

    void *result = realloc(pointer, new_size);
    if (result == NULL) exit(1); // oom. handle better later
    return result;
}

void *reallocate2(void *pointer, size_t old_size, size_t new_size) {
    if (new_size == 0) {
        free(pointer);
        return NULL;
    }

    void *result = realloc(pointer, new_size);
    if (result == NULL) exit(1); // oom. handle better later

    // Zero the new memory
    if (new_size > old_size) {
        size_t diff = new_size - old_size;
        void* pStart = ((char*) result) + old_size;
        memset(pStart, 0, diff);
    }

    return result;
}

#define self_t IndexList *self

void index_list_init(self_t) {
    self->size = 0;
    self->capacity = 0;
    self->data = NULL;
}

void index_list_free(self_t) {
    ARRAY_FREE(uint32_t, self->data);
    index_list_init(self);
}

void index_list_add(self_t, uint32_t index) {
    if (self->capacity < self->size + 1) {
        self->capacity = ARRAY_GROW_CAPCITY(self->capacity);
        self->data = ARRAY_GROW(uint32_t, self->data, self->capacity);
    }

    self->data[self->size] = index;
    self->size++;
}

uint32_t index_list_add_multi(self_t, void *data, size_t size) {
    // Size may not be greater than 8 because then ARRAY_GROW_CAPACITY is not guaranteed to allocate enough new memory.
    // The initial size is 8, and will always double after that.
    assert(size <= 8);

    if (self->capacity < self->size + size) {
        self->capacity = ARRAY_GROW_CAPCITY(self->capacity);
        self->data = ARRAY_GROW(uint32_t, self->data, self->capacity);
    }

    memcpy(self->data + self->size, data, size * sizeof(uint32_t));
    uint32_t start_index = self->size;
    self->size += size;
    return start_index;
}

uint32_t *index_list_get(self_t, uint32_t i) {
    if (i < self->size)
        return &self->data[i];
    return NULL;
}

bool index_list_contains(self_t, uint32_t index) {
    for (uint32_t i = 0; i < self->size; i++) {
        if (self->data[i] == index)
            return true;
    }
    return false;
}

#undef self_t

#define self_t IndexMap *self

void index_map_init(self_t) {
    self->capacity = 0;
    self->data = NULL;
}

void index_map_free(self_t) {
    ARRAY_FREE(uint32_t, self->data);
    index_map_init(self);
}

void index_map_put(self_t, uint32_t key, uint32_t value) {
    if (self->capacity < key + 1) {
        uint32_t old_capacity = self->capacity;
        self->capacity = ARRAY_GROW_CAPCITY(key + 1);
        self->data = ARRAY_GROW2(uint32_t, self->data, old_capacity, self->capacity);
    }

    self->data[key] = value;
}

uint32_t *index_map_get(self_t, uint32_t key) {
    if (key < self->capacity)
        return &self->data[key];
    return 0;
}

#undef self_t

#define self_t IndexPtrMap *self

void index_ptr_map_init(self_t) {
    self->capacity = 0;
    self->data = NULL;
}

void index_ptr_map_free(self_t) {
    ARRAY_FREE(uint32_t, self->data);
    index_ptr_map_init(self);
}

void index_ptr_map_put(self_t, uint32_t key, size_t value) {
    if (self->capacity < key + 1) {
        uint32_t old_capacity = self->capacity;
        self->capacity = ARRAY_GROW_CAPCITY(key + 1);
        self->data = ARRAY_GROW2(size_t, self->data, old_capacity, self->capacity);
    }

    self->data[key] = value;
}

size_t *index_ptr_map_get(self_t, uint32_t key) {
    if (key < self->capacity)
        return &self->data[key];
    return 0;
}

#undef self_t
