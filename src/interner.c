#include <string.h>
#include "interner.h"
#include "array_util.h"

#define self_t StringSet *self

void string_set_init(self_t) {
    self->size = 0;
    self->capacity = 0;
    self->data = NULL;
}

void string_set_free(self_t) {
    ARRAY_FREE(uint32_t, self->data);
    string_set_init(self);
}

static StringKey string_set_add_new(self_t, char *string) {
    if (self->capacity < self->size + 1) {
        self->capacity = ARRAY_GROW_CAPCITY(self->capacity);
        self->data = ARRAY_GROW(char *, self->data, self->capacity);
    }

    self->data[self->size] = string;
    self->size++;
    return self->size - 1;
}

StringKey string_set_add(self_t, char *string) {
    assert(string != NULL);

    for (uint32_t i = 0; i < self->size; i++) {
        if (strcmp(self->data[i], string) == 0) {
            return i;
        }
    }

    return string_set_add_new(self, string);
}

char *string_set_get(self_t, StringKey key) {
    assert(key < self->size);
    return self->data[key];
}

#undef self_t
