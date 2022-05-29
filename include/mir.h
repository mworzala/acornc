#ifndef CONFIG_MIR_H
#define CONFIG_MIR_H

#include "common.h"
#include "ir_common.h"
#include "array_util.h"

typedef uint32_t MirIndex;

#define mir_index_empty (0)

typedef enum mir_inst_tag_s {
    MirReserved, // Does not represent any content, just for placeholding.
    MirConstant,

    // ty_pl pointing to Block
    MirBlock,
    // un_op, no instructions may follow within a block (todo implement that error)
    MirRet,

    __MIR_LAST,
} MirInstTag;

char *mir_tag_to_string(MirInstTag tag);

// 8 bytes max
typedef union mir_inst_data_s {
    Ref un_op;
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
