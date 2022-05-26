#include "mir.h"

#include "array_util.h"

#define self_t MirInstList *self

void mir_inst_list_init(self_t) {
    self->size = 0;
    self->capacity = 0;
    self->data = NULL;
}

void mir_inst_list_free(self_t) {
    ARRAY_FREE(MirInst, self->data);
    mir_inst_list_init(self);
}

void mir_inst_list_add(self_t, MirInst inst) {
    if (self->capacity < self->size + 1) {
        self->capacity = ARRAY_GROW_CAPCITY(self->capacity);
        self->data = ARRAY_GROW(MirInst, self->data, self->capacity);
    }

    self->data[self->size] = inst;
    self->size++;
}

MirInst *mir_inst_list_get(self_t, uint32_t index) {
    if (index < self->size)
        return &self->data[index];
    return NULL;
}

#undef self_t
