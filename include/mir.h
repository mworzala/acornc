#ifndef CONFIG_MIR_H
#define CONFIG_MIR_H

#include "common.h"
#include "ir_common.h"
#include "array_util.h"

typedef uint32_t MirIndex;

#define mir_index_empty (0)

typedef enum mir_inst_tag_s {
    MirReserved, // Does not represent any content, just for placeholding.

    // Integer addition
    // bin_op
    MirAdd,
    // Allocates an int on the stack
    // data is noop
    MirAlloc,
    // ty_pl pointing to Block
    // Note: MirBlock is different from an AstBlock. AstBlocks are valid expressions,
    //       however MirBlock may only be present where branches happen (eg if, while, etc)
    MirBlock,
    // ty_pl where pl represents the 32 bit content of the int
    // todo actual type, allow bigger ints.
    MirConstant,
    // Integer division
    // bin_op
    MirDiv,
    // Loads a value from the given location
    // un_op where payload is pointer
    MirLoad,
    // Integer multiplication
    // bin_op
    MirMul,
    // un_op, no instructions may follow within a block (todo implement that error)
    MirRet,
    // Stores a value to the given location
    // bin_op where lhs is pointer, rhs is value
    MirStore,
    // Integer subtraction
    // bin_op
    MirSub,

    __MIR_LAST,
} MirInstTag;

char *mir_tag_to_string(MirInstTag tag);

// 8 bytes max
typedef union mir_inst_data_s {
    uint8_t noop;
    Ref un_op;
    struct {
        Ref lhs;
        Ref rhs;
    } bin_op;
    //todo ty
    struct {
        // Index in instructions, extra, or values
        uint32_t payload;
    } ty_pl;
} MirInstData;

// Data payloads

// Followed by inst_count instructions inside the block
typedef struct {
    uint32_t inst_count;
} Block;

typedef struct mir_inst_s {
    MirInstTag tag;
    MirInstData data;
} MirInst;

typedef struct mir_inst_list_s {
    uint32_t size;
    uint32_t capacity;
    MirInst *data;
} MirInstList;

#define self_t MirInstList *self

void mir_inst_list_init(self_t);
void mir_inst_list_free(self_t);
void mir_inst_list_add(self_t, MirInst inst);
MirInst *mir_inst_list_get(self_t, uint32_t index);

#undef self_t

typedef struct mir_s {
    MirInstList instructions;
    IndexList extra;
    // extra u32 list todo refactor ast list to list util (they are all u32)
    // values value list todo value union
} Mir;

#define self_t Mir *self

void mir_add_inst(self_t, MirInstTag tag, MirInstData data);

#undef self_t

#endif //CONFIG_MIR_H
