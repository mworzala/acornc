#ifndef ACORN_HIR_H
#define ACORN_HIR_H

#include "common.h"
#include "array_util.h"
#include "interner.h"

typedef uint32_t HirIndex;

#define hir_index_empty (0)

/*
 * TODOS
 * - AST_DOT (?)
 * - AST_CALL
 * - AST_STRUCT
 * - AST_ENUM
 */

typedef enum hir_inst_tag_s {
    HIR_RESERVED, // Does not represent any content, just for placeholding.

    // Represents an integer if the size fits into 64 bits. Added to HIR
    HIR_INT,
    // Uses `str_value`.
    HIR_STRING,
    // Uses `int_value`, either 1 or 0
    HIR_BOOL,
    // Uses `un_op` referencing the index of the referenced element (let, fn param, etc)
    HIR_REF,

    // All use `bin_op`
    HIR_ADD,
    HIR_SUB,
    HIR_MUL,
    HIR_DIV,
    HIR_CMP_EQ,
    HIR_CMP_NE,
    HIR_CMP_LT,
    HIR_CMP_LE,
    HIR_CMP_GT,
    HIR_CMP_GE,
    HIR_AND,
    HIR_OR,

    // May only contain a single expression, which is treated as the "return" from the block
    // Uses `un_op`
    HIR_BLOCK_INLINE,
    // Always contains an expresion, uses un_op.
    HIR_BREAK_INLINE,
    // Contains a number of expressions
    // Uses `extra` pointing to `HirBlock`
    HIR_BLOCK,
    // May contain an expression, uses un_op. If zero no expression is present
    HIR_RETURN,

    // `extra` points to HirCond
    HIR_COND,
    // `extra` points to HirLoop
    HIR_LOOP,

    // Uses `un_op` pointing to the init expr
    HIR_LET,

    // Represents a const declaration within a module
    // pl_op where pl is the name of the const (in the intern table), and op is the value of the const
    // The value can be either a block_inline or a fn_decl
    HIR_CONST_DECL,
    // `extra` pointing to a `HirFnDecl`
    HIR_FN_DECL,
    // `pl_op` with name and type expr
    HIR_FN_PARAM,

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

// 8 bytes max, if something needs to be bigger use `extra`
typedef union hir_inst_data_s {
    uint8_t noop;
    uint64_t int_value;
    StringKey str_value;
    uint32_t extra;
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

typedef struct hir_cond_s {
    HirIndex condition;
    HirIndex then_branch;
    HirIndex else_branch;
} HirCond;

typedef struct hir_loop_s {
    HirIndex condition;
    HirIndex body;
} HirLoop;

typedef struct hir_block_s {
    uint32_t len; // One instruction index follows for each `len`
} HirBlock;

typedef struct hir_fn_decl_s {
    uint32_t flags;
    HirIndex ret_ty; // If 0, return type is void, otherwise a type index
    uint32_t param_len; // One instruction follows for each parameter (points to a HIR_PARAM)
    uint32_t body; // body block instruction
} HirFnDecl;

#define HIR_FN_DECL_FLAGS_NONE (0x0)
#define HIR_FN_DECL_FLAGS_FOREIGN (0x1)

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
