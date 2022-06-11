#ifndef CONFIG_INTERNER_H
#define CONFIG_INTERNER_H

#include "common.h"

typedef uint32_t StringKey;

typedef struct string_set_s {
    uint32_t size;
    uint32_t capacity;
    char **data;
} StringSet;

#define self_t StringSet *self

void string_set_init(self_t);
void string_set_free(self_t);
StringKey string_set_add(self_t, char *string);
char *index_list_get(self_t, StringKey key);

#undef self_t

#endif //CONFIG_INTERNER_H
