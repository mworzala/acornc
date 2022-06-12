#include "error.h"

#include <stdlib.h>

#include "array_util.h"

#define self_t ErrorList *self

void error_list_init(self_t) {
    self->size = 0;
    self->capacity = 0;
    self->data = NULL;
}

void error_list_free(self_t) {
    //todo free all errors

    ARRAY_FREE(uint32_t, self->data);
    error_list_init(self);
}

void error_list_add(self_t, CompileError error) {
    if (self->capacity < self->size + 1) {
        self->capacity = ARRAY_GROW_CAPCITY(self->capacity);
        self->data = ARRAY_GROW(CompileError *, self->data, self->capacity);
    }

    CompileError *owned = malloc(sizeof(CompileError));
    *owned = error;

    self->data[self->size] = owned;
    self->size++;
}

CompileError *error_list_get(self_t, uint32_t index) {
    if (index < self->size)
        return self->data[index];
    return NULL;
}

#undef self_t