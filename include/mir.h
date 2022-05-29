#ifndef CONFIG_MIR_H
#define CONFIG_MIR_H

#include "common.h"
#include "ir_common.h"
#include "array_util.h"

typedef uint32_t AirIndex;

typedef enum mir_inst_tag_s {
    MirInt,
} MirInstTag;

// 8 bytes max
typedef union mir_inst_data_s {

} MirInstData;

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
