#ifndef ACORN_HIR_H
#define ACORN_HIR_H

#include "common.h"
#include "array_util.h"
#include "interner.h"

typedef uint32_t HirIndex;

#define hir_index_empty (0)

typedef enum hir_inst_tag_s {
    HIR_RESERVED, // Does not represent any content, just for placeholding.

    // Represents an integer if the size fits into 64 bits. Added to HIR
    HIR_INT,

    // Represents a const declaration within a module
    // pl_op where pl is the name of the const (in the intern table), and op is the value of the const
    HIR_CONST_DECL,

    // Represents a module (file)
    //todo how is this represented?
    HIR_MODULE,

    __HIR_LAST,
} HirInstTag;

char *hir_tag_to_string(HirInstTag tag);

// 8 bytes max
typedef union hir_inst_data_s {
    uint8_t noop;
    uint64_t int_value;
    struct {
        uint32_t payload;
        HirIndex operand;
    } pl_op;
} HirInstData;

typedef struct hir_inst_s {
    HirInstTag tag;
    HirInstData data;
} HirInst;

typedef struct Hir_inst_list_s {
    uint32_t size;
    uint32_t capacity;
    HirInst *data;
} HirInstList;

#define self_t HirInstList *self

void hir_inst_list_init(self_t);
void hir_inst_list_free(self_t);
void hir_inst_list_add(self_t, HirInst inst);
HirInst *hir_inst_list_get(self_t, uint32_t index);

#undef self_t

typedef struct hir_s {
    HirInstList instructions;
    IndexList extra;
    StringSet strings;
} Hir;

#define self_t Hir *self

HirInst *hir_get_inst(self_t, HirIndex index);
HirInst *hir_get_inst_tagged(self_t, HirIndex index, HirInstTag tag);

#undef self_t

#endif //ACORN_HIR_H
