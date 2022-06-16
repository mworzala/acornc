#include <stdlib.h>
#include <string.h>
#include "debug/hir_debug.h"

typedef struct {
    Hir *hir;

    char *buffer;
    size_t buffer_index;
} HirDebug;

#define HIR_DEBUG_BUFFER_SIZE (sizeof(char) * 4096)

#define self_t HirDebug *self


// SECTION: Utilities
// Utility macros and functions for retrieving or emitting information.

#define print(self, ...) { \
    sprintf(self->buffer + self->buffer_index, __VA_ARGS__); \
    self->buffer_index += strlen(self->buffer + self->buffer_index); \
}

#define get_extra(self, index) *index_list_get(&(self)->ast->extra_data, (index))

void print_string(self_t, StringKey key, bool quote) {
    // str still owned by string set
    char *str = string_set_get(&self->hir->strings, key);
    if (quote) { print(self, "\"%s\"", str); }
    else       { print(self, "%s", str); }
}

void print_indent(self_t, int indent) {
    print(self, "%*s", indent, "");
}

void print_default_header(self_t, HirIndex index, int indent) {
    print_indent(self, indent);
    print(self, "%%%d = ", index);
}


// SECTION: Forward declarations
// Any elements which need forward declarations are done here

static void print_inst(self_t, HirIndex index, int indent);

static void print_inst_if_present(self_t, HirIndex index, int indent) {
    if (index == hir_index_empty)
        return;
    print_inst(self, index, indent);
}


// SECTION: Implementation
// Debug implementation handled here.

static void print_const_decl(self_t, HirIndex index, HirInst *inst, int indent) {
    print_indent(self, indent);

    // Print const name
    print_string(self, inst->data.pl_op.payload, false);

    print(self, ": %%%d = ", index);
    print(self, "todo({\n");

    // Print initializer
    print_inst(self, inst->data.pl_op.operand, indent + 2);

    print_indent(self, indent);
    print(self, "})\n");
}

static void print_int(self_t, HirIndex index, HirInst *inst, int indent) {
    print_default_header(self, index, indent);
    print(self, "int(%llu)\n", inst->data.int_value);
}


static void print_inst(self_t, HirIndex index, int indent) {
    HirInst *inst = hir_get_inst(self->hir, index);
    assert(inst != NULL);

    switch (inst->tag) {
        case HIR_MODULE:
            assert(false);

        case HIR_CONST_DECL:
            print_const_decl(self, index, inst, indent);
            break;

        case HIR_INT:
            print_int(self, index, inst, indent);
            break;

        default:
            assert(false);
    }
}

#undef self_t

char *hir_debug_print(Hir *hir, HirIndex root) {
    HirDebug self = {
        .hir = hir,
        .buffer = malloc(HIR_DEBUG_BUFFER_SIZE),
        .buffer_index = 0,
    };
    memset(self.buffer, 0, HIR_DEBUG_BUFFER_SIZE);

    print_inst(&self, root, 0);

    return self.buffer;
}
