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
    // Uses `str_value`.
    HIR_STRING,
    // Uses `int_value`, either 1 or 0
    HIR_BOOL,

    // All use `bin_op`
    HIR_ADD,
    HIR_SUB,
    HIR_MUL,
    HIR_DIV,

    // Represents a const declaration within a module
    // pl_op where pl is the name of the const (in the intern table), and op is the value of the const
    HIR_CONST_DECL,

    // Represents a module (file)
    //todo how is this represented?
    HIR_MODULE,

    // Ensures the inner expression matches the given type.
    // pl_op where pl is the type, and op is the expression to match.
    HIR_AS_TYPE,

    // Represents a type expression (for now a static name (eg i32, bool) optionally prefixed with a ptr)
    // ty
    //todo dedicated tests
    HIR_TYPE,

    __HIR_LAST,
} HirInstTag;

char *hir_tag_to_string(HirInstTag tag);

// 8 bytes max
typedef union hir_inst_data_s {
    uint8_t noop;
    uint64_t int_value;
    StringKey str_value;
    struct {
        uint32_t payload;
        HirIndex operand;
    } pl_op;
    struct {
        bool is_ptr;
        // if `is_ptr`, inner is a HirIndex pointing to the pointed type
        // otherwise, inner is a StringKey representing the type name
        uint32_t inner;
    } ty;
    HirIndex un_op;
    struct {
        HirIndex lhs;
        HirIndex rhs;
    } bin_op;
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
