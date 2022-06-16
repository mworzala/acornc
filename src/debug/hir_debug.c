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

#define get_extra(self, index) *index_list_get(&(self)->hir->extra, (index))

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

    print(self, ": ");

    // Print content
    print_inst(self, inst->data.pl_op.operand, 0);
}

static void print_int(self_t, HirIndex index, HirInst *inst, int indent) {
    print_default_header(self, index, indent);
    print(self, "int(%llu)\n", inst->data.int_value);
}

static void print_string_inst(self_t, HirIndex index, HirInst *inst, int indent) {
    print_default_header(self, index, indent);
    print(self, "str(");
    print_string(self, inst->data.str_value, true);
    print(self, ")\n");
}

static void print_bool(self_t, HirIndex index, HirInst *inst, int indent) {
    print_default_header(self, index, indent);
    print(self, "bool(");
    print(self, inst->data.int_value ? "true" : "false");
    print(self, ")\n");
}

static void print_binary_generic(self_t, char *name, HirIndex index, HirInst *inst, int indent) {
    print_inst(self, inst->data.bin_op.lhs, indent);
    print_inst(self, inst->data.bin_op.rhs, indent);

    print_default_header(self, index, indent);
    print(self, "%s(%%%d, %%%d)\n", name, inst->data.bin_op.lhs, inst->data.bin_op.rhs);
}

static void print_block_inline(self_t, HirIndex index, HirInst *inst, int indent) {
    print_default_header(self, index, indent);

    print(self, "block_inline({\n");

    print_inst(self, inst->data.un_op, indent + 2);

    print_indent(self, indent);
    print(self, "})\n");
}

static void print_block(self_t, HirIndex index, HirInst *inst, int indent) {
    print_default_header(self, index, indent);
    print(self, "block(");

    HirBlock *block_data = index_list_get_sized(&self->hir->extra, HirBlock, inst->data.extra);
    assert(block_data != NULL);

    // Exit early if block is empty
    if (block_data->len == 0) {
        print(self, ")\n");
        return;
    }

    // Print each statement
    print(self, "{\n");

    for (size_t i = 0; i < block_data->len; i++) {
        HirIndex stmt_index = get_extra(self, inst->data.extra + i + 1);
        print_inst(self, stmt_index, indent + 2);
        if (i < block_data->len - 1)
            print(self, "\n");
    }

    print_indent(self, indent);
    print(self, "})\n");
}

static void print_as_type(self_t, HirIndex index, HirInst *inst, int indent) {
    print_inst(self, inst->data.pl_op.operand, indent);

    print_default_header(self, index, indent);
    print(self, "as_type(");
    print_inst(self, inst->data.pl_op.payload, 0);
    print(self, ", %%%d)\n", inst->data.pl_op.operand);
}

static void print_type(self_t, HirIndex index, HirInst *inst, int indent) {
    if (inst->data.ty.is_ptr) {
        // This is a pointer, just print a * and inner
        print(self, "*")
        print_inst(self, inst->data.ty.inner, 0);
        return;
    }

    // Not a pointer, print the type name
    print_string(self, inst->data.ty.inner, false);
}


static void print_inst(self_t, HirIndex index, int indent) {
    HirInst *inst = hir_get_inst(self->hir, index);
    assert(inst != NULL);

    switch (inst->tag) {
        case HIR_MODULE:        assert(false);

        case HIR_CONST_DECL:    print_const_decl(self, index, inst, indent); break;

        case HIR_INT:           print_int(self, index, inst, indent); break;
        case HIR_STRING:        print_string_inst(self, index, inst, indent); break;
        case HIR_BOOL:          print_bool(self, index, inst, indent); break;

        case HIR_ADD:           print_binary_generic(self, "add", index, inst, indent); break;
        case HIR_SUB:           print_binary_generic(self, "sub", index, inst, indent); break;
        case HIR_MUL:           print_binary_generic(self, "mul", index, inst, indent); break;
        case HIR_DIV:           print_binary_generic(self, "div", index, inst, indent); break;

        case HIR_BLOCK_INLINE:  print_block_inline(self, index, inst, indent); break;
        case HIR_BLOCK:         print_block(self, index, inst, indent); break;

        case HIR_AS_TYPE:       print_as_type(self, index, inst, indent); break;
        case HIR_TYPE:          print_type(self, index, inst, indent); break;

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
