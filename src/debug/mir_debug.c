#include "debug/mir_debug.h"

#include <stdlib.h>
#include <string.h>
#include "array_util.h"

typedef struct {
    Mir *mir;
    IndexList visited;

    char *buffer;
    size_t buffer_index;
} MirDebug;

#define self_t MirDebug *self

#define print(self, ...) { \
    sprintf(self->buffer + self->buffer_index, __VA_ARGS__); \
    self->buffer_index += strlen(self->buffer + self->buffer_index); \
}

#define get_inst(self, index) mir_inst_list_get(&(self)->mir->instructions, (index))
#define get_extra(self, index) *index_list_get(&(self)->mir->extra, (index))

static inline MirInst *get_inst_tagged(self_t, MirIndex index, MirInstTag tag) {
    MirInst *inst = get_inst(self, index);
    assert(inst->tag == tag);
    return inst;
}

static void append_default_header(self_t, MirIndex index, int indent) {
    print(self, "%*s%%%d = ", indent, "", index)
}

static void print_ref(self_t, Ref ref) {
    if (ref > __REF_LAST) {
        print(self, "%%%d", ref_to_index(ref));
    } else {
        print(self, "@ref.%s", ref_to_string(ref));
    }
}



static void print_block_inst(self_t, MirIndex index, int indent);

static void print_block_inst_ref(self_t, Ref ref, int indent) {
    if (ref > __REF_LAST) {
        MirIndex index = ref_to_index(ref);

        bool visited = index_list_contains(&self->visited, index);
        if (visited) return;

        print_block_inst(self, index, indent);
        index_list_add(&self->visited, index);
    }
}

static void print_constant(self_t, MirIndex index, int indent) {
    MirInst *inst = get_inst_tagged(self, index, MirConstant);
    append_default_header(self, index, indent);

    print(self, "constant(i32, %d)", inst->data.ty_pl.payload);
}

static void print_alloc(self_t, MirIndex index, int indent) {
    MirInst *inst = get_inst_tagged(self, index, MirAlloc);

    append_default_header(self, index, indent);
    print(self, "alloc(i32)") //todo types
}

static void print_store(self_t, MirIndex index, int indent) {
    MirInst *inst = get_inst_tagged(self, index, MirStore);

    print_block_inst_ref(self, inst->data.bin_op.lhs, indent);
    print_block_inst_ref(self, inst->data.bin_op.rhs, indent);

    append_default_header(self, index, indent);
    print(self, "store(")
    print_ref(self, inst->data.bin_op.lhs);
    print(self, ", ")
    print_ref(self, inst->data.bin_op.rhs);
    print(self, ")")
}

static void print_load(self_t, MirIndex index, int indent) {
    MirInst *inst = get_inst_tagged(self, index, MirLoad);

    print_block_inst_ref(self, inst->data.un_op, indent);

    append_default_header(self, index, indent);
    print(self, "load(")
    print_ref(self, inst->data.un_op);
    print(self, ")")
}

static void print_ret(self_t, MirIndex index, int indent) {
    MirInst *inst = get_inst_tagged(self, index, MirRet);
    print_block_inst_ref(self, inst->data.un_op, indent);

    append_default_header(self, index, indent);
    print(self, "ret(")
    print_ref(self, inst->data.un_op);
    print(self, ")");
}

static void print_block_inst(self_t, MirIndex index, int indent) {
    MirInst *inst = get_inst(self, index);

    switch (inst->tag) {
        case MirConstant:
            print_constant(self, index, indent);
            break;
        case MirAlloc:
            print_alloc(self, index, indent);
            break;
        case MirStore:
            print_store(self, index, indent);
            break;
        case MirLoad:
            print_load(self, index, indent);
            break;
        case MirRet:
            print_ret(self, index, indent);
            break;
        case MirReserved:
            printf("Illegal reserved tag present in MIR\n");
            assert(false);
        default:
            printf("Unhandled tag: %s\n", mir_tag_to_string(inst->tag));
            assert(false);
    }
    print(self, "\n")
}


static void print_root_block(self_t) {
    MirInst *inst = get_inst(self, 0);
    assert(inst->tag == MirBlock);

    MirIndex data_index = inst->data.ty_pl.payload;
    uint32_t expr_count = get_extra(self, data_index);

    for (uint32_t i = data_index + 1; i <= data_index + expr_count; i++) {
        print_block_inst(self, get_extra(self, i), 0);
    }

    print(self, "\n")
}

#undef self_t

char *mir_debug_print(Mir *mir) {
    char *buffer = malloc(4096);
    memset(buffer, 0, 4096);

    MirDebug self = {
        .mir = mir,
        .buffer = buffer,
        .buffer_index = 0,
    };
    index_list_init(&self.visited);

    print_root_block(&self);

    index_list_free(&self.visited);

    return buffer;
}
