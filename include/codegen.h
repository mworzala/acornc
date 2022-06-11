#ifndef CONFIG_CODEGEN_H
#define CONFIG_CODEGEN_H

#include "common.h"
#include "mir.h"
#include "llvm-c/Core.h"

// SECTION: Codegen
// LLVM IR generation for a single module and its declarations

typedef struct module_s Module;
typedef struct decl_s Decl;

typedef struct codegen_s {
    Module *module;

    LLVMContextRef ll_context;
    LLVMModuleRef ll_module;
    LLVMBuilderRef ll_builder;

    // Current MIR being generated
    Mir *mir;
    // A mapping between MIR instructions and LLVM instructions
    IndexPtrMap inst_map;
    // Set to whatever function is currently being generated
    LLVMValueRef *curr_fn;
} Codegen;

#define self_t Codegen *self

void codegen_init(self_t, Module *module);
void codegen_free(self_t);

bool codegen_write_to_file(self_t, char *path);
bool codegen_write_to_obj_file(self_t, char *path);

void codegen_lower_decl(self_t, Decl *decl);
LLVMTypeRef codegen_fn_proto(self_t, Decl *decl);

LLVMTypeRef codegen_type_to_llvm(self_t, Type type);

LLVMValueRef codegen_inst(self_t, MirIndex index, LLVMBasicBlockRef ll_block);
LLVMValueRef codegen_constant(self_t, MirIndex index, LLVMBasicBlockRef ll_block);
LLVMValueRef codegen_binary_op(self_t, MirIndex index, LLVMBasicBlockRef ll_block);
LLVMValueRef codegen_alloc(self_t, MirIndex index, LLVMBasicBlockRef ll_block);
void codegen_store(self_t, MirIndex index, LLVMBasicBlockRef ll_block);
LLVMValueRef codegen_load(self_t, MirIndex index, LLVMBasicBlockRef ll_block);
LLVMValueRef codegen_call(self_t, MirIndex index, LLVMBasicBlockRef ll_block);
LLVMValueRef codegen_arg(self_t, MirIndex index, LLVMBasicBlockRef ll_block);
LLVMValueRef codegen_fn_ptr(self_t, MirIndex index);
void codegen_return(self_t, MirInst *inst, LLVMBasicBlockRef ll_block);
void codegen_block_direct(self_t, MirIndex block_index, LLVMBasicBlockRef ll_block);


#undef self_t

#endif //CONFIG_CODEGEN_H
