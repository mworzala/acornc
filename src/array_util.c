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

#define self_t IndexList *self

void index_list_init(self_t) {
    self->size = 0;
    self->capacity = 0;
    self->data = NULL;
}

void index_list_free(self_t) {
    ARRAY_FREE(AstNode, self->data);
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

void index_list_add_multi(self_t, void *data, size_t size) {
    // Size may not be greater than 8 because then ARRAY_GROW_CAPACITY is not guaranteed to allocate enough new memory.
    // The initial size is 8, and will always double after that.
    assert(size <= 8);

    if (self->capacity < self->size + size) {
        self->capacity = ARRAY_GROW_CAPCITY(self->capacity);
        self->data = ARRAY_GROW(uint32_t, self->data, self->capacity);
    }

    memcpy(self->data + self->size, data, size * sizeof(uint32_t));
    self->size += size;
}

uint32_t *index_list_get(self_t, uint32_t i) {
    if (i < self->size)
        return &self->data[i];
    return NULL;
}

#undef self_t
