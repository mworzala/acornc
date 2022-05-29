#include <stdlib.h>
#include "codegen.h"
#include <llvm-c/Target.h>
#include <llvm-c/TargetMachine.h>
#include <string.h>
#include <unistd.h>

#include "module.h"

//todo put me a better place, it is duplicated from mir_debug.c
#define mir_get_inst(mir, index) mir_inst_list_get(&(mir)->instructions, (index))
#define mir_get_extra(mir, index) *index_list_get(&(mir)->extra, (index))

static inline MirInst *mir_get_inst_tagged(Mir *mir, MirIndex index, MirInstTag tag) {
    MirInst *inst = mir_get_inst(mir, index);
    assert(inst->tag == tag);
    return inst;
}

#define self_t Codegen *self

#define llvm_int LLVMInt64TypeInContext(self->ll_context)

void codegen_init(self_t, Module *module) {
    assert(module != NULL);
    self->module = module;

    self->ll_context = LLVMContextCreate();
    self->ll_module = LLVMModuleCreateWithNameInContext(module->name, self->ll_context);
    self->ll_builder = LLVMCreateBuilderInContext(self->ll_context);

    self->curr_fn = NULL;
    index_ptr_map_init(&self->inst_map);
}

void codegen_free(self_t) {

    LLVMDisposeBuilder(self->ll_builder);
    self->ll_builder = NULL;
    LLVMDisposeModule(self->ll_module);
    self->ll_module = NULL;
    LLVMContextDispose(self->ll_context);
    self->ll_context = NULL;

    self->module = NULL;
}

bool codegen_write_to_file(self_t, char *path) {
    char *error = NULL;
    bool has_error = LLVMPrintModuleToFile(self->ll_module, path, &error);
    if (has_error) {
        fprintf(stderr, "Error writing to file: %s\n", error);
        LLVMDisposeMessage(error);
    }

    return !has_error;
}

bool codegen_write_to_obj_file(self_t, char *path) {
    char *errors = NULL;

    LLVMInitializeAllTargetInfos();
    LLVMInitializeAllTargets();
    LLVMInitializeAllTargetMCs();
    LLVMInitializeAllAsmParsers();
    LLVMInitializeAllAsmPrinters();

    LLVMTargetRef target;
    bool has_error = LLVMGetTargetFromTriple(LLVMGetDefaultTargetTriple(), &target, &errors);
    if (has_error) printf("error: %s\n", errors);
    LLVMDisposeMessage(errors);
    LLVMTargetMachineRef machine = LLVMCreateTargetMachine(target, LLVMGetDefaultTargetTriple(), "generic", LLVMGetHostCPUFeatures(), LLVMCodeGenLevelDefault, LLVMRelocDefault, LLVMCodeModelDefault);

    LLVMSetTarget(self->ll_module, LLVMGetDefaultTargetTriple());
    LLVMTargetDataRef datalayout = LLVMCreateTargetDataLayout(machine);
    char* datalayout_str = LLVMCopyStringRepOfTargetData(datalayout);
    LLVMSetDataLayout(self->ll_module, datalayout_str);
    LLVMDisposeMessage(datalayout_str);

    has_error = LLVMTargetMachineEmitToFile(machine, self->ll_module, path, LLVMObjectFile, &errors);
    if (has_error) printf("error: %s\n", errors);
    LLVMDisposeMessage(errors);

    // Strip .acorn.o from path
    char *path2 = strdup(path);
    char *dot_acorn = strstr(path2, ".acorn");
    if (dot_acorn) {
        *dot_acorn = '\0';
    }

    int exit_code = execl("/usr/bin/cc", "cc", path, "-o", path2, NULL);
    free(path2);

    if (exit_code != 0) {
        printf("cc finished with non-zero exit code: %d\n", exit_code);
        return false;
    }

    return true;
}

void codegen_lower_decl(self_t, Decl *decl) {
    assert(decl != NULL);

    LLVMTypeRef fn_type = codegen_fn_proto(self, decl);
    LLVMValueRef fn = LLVMAddFunction(self->ll_module, decl->name, fn_type);
    self->curr_fn = &fn;

    Mir *mir = decl_get_mir_in_module(decl, self->module);
    self->mir = mir;
    index_ptr_map_init(&self->inst_map);

    LLVMBasicBlockRef entry_block = LLVMAppendBasicBlock(fn, "entry");
    LLVMPositionBuilderAtEnd(self->ll_builder, entry_block);

    codegen_block_direct(self, 0, entry_block);

    index_ptr_map_free(&self->inst_map);
    self->mir = NULL;
    self->curr_fn = NULL;
}

LLVMTypeRef codegen_fn_proto(self_t, Decl *decl) {
    AstNode *fn_ast = ast_get_node_tagged(self->module->ast, decl->ast_index, AST_NAMED_FN);
    AstNode *proto_ast = ast_get_node_tagged(self->module->ast, fn_ast->data.lhs, AST_FN_PROTO);
    AstFnProto proto = *((AstFnProto *) &self->module->ast->extra_data.data[proto_ast->data.lhs]);

    size_t param_count = 0;
    LLVMTypeRef *params = NULL;
    if (proto.param_start != ast_index_empty) {
        param_count = proto.param_end - proto.param_start + 1;
        params = malloc(sizeof(LLVMTypeRef) * param_count);
        for (int32_t i = 0; i < param_count; i++)
            params[i] = llvm_int;
    }

    LLVMTypeRef fn_type = LLVMFunctionType(llvm_int, params, param_count, false);

    if (params != NULL) {
        free(params);
    }

    return fn_type;
}


LLVMValueRef codegen_inst(self_t, MirIndex index, LLVMBasicBlockRef ll_block) {
    LLVMValueRef *ll_inst = (LLVMValueRef *) index_ptr_map_get(&self->inst_map, index);
    if (ll_inst != NULL && *ll_inst != NULL) {
        return *ll_inst;
    }

    MirInst *inst = mir_get_inst(self->mir, index);
    LLVMValueRef ll_value;
    switch (inst->tag) {
        case MirConstant: {
            ll_value = codegen_constant(self, inst);
            break;
        }
        case MirAlloc: {
            ll_value = codegen_alloc(self, index, ll_block);
            break;
        }
        case MirStore:
            codegen_store(self, index, ll_block);
            return NULL;
        case MirLoad: {
            ll_value = codegen_load(self, index, ll_block);
            break;
        }
        case MirRet: {
            codegen_return(self, inst, ll_block);
            return NULL;
        }
        case MirReserved: {
            printf("Illegal reserved tag present in MIR\n");
            assert(false);
        }
        default: {
            printf("Unhandled tag for codegen: %s\n", mir_tag_to_string(inst->tag));
            assert(false);
        }
    }

    index_ptr_map_put(&self->inst_map, index, (size_t) ll_value);

    return ll_value;
}

LLVMValueRef codegen_constant(self_t, MirInst *inst) {
    return LLVMConstInt(llvm_int, inst->data.ty_pl.payload, false);
}

LLVMValueRef codegen_alloc(self_t, MirIndex index, LLVMBasicBlockRef ll_block) {
    MirInst *inst = mir_get_inst_tagged(self->mir, index, MirAlloc);

    return LLVMBuildAlloca(self->ll_builder, llvm_int, "alloc"); //todo preserve name somehow
}

void codegen_store(self_t, MirIndex index, LLVMBasicBlockRef ll_block) {
    MirInst *inst = mir_get_inst_tagged(self->mir, index, MirStore);

    LLVMValueRef ptr = codegen_inst(self, ref_to_index(inst->data.bin_op.lhs), ll_block);
    LLVMValueRef value = codegen_inst(self, ref_to_index(inst->data.bin_op.rhs), ll_block);

    LLVMBuildStore(self->ll_builder, value, ptr);
}

LLVMValueRef codegen_load(self_t, MirIndex index, LLVMBasicBlockRef ll_block) {
    MirInst *inst = mir_get_inst_tagged(self->mir, index, MirLoad);

    LLVMValueRef ptr = codegen_inst(self, ref_to_index(inst->data.un_op), ll_block);

    return LLVMBuildLoad(self->ll_builder, ptr, "load");
}

void codegen_return(self_t, MirInst *inst, LLVMBasicBlockRef ll_block) {
    //todo codegen for refs
    LLVMValueRef ret_val = codegen_inst(self, ref_to_index(inst->data.un_op), ll_block);
    LLVMBuildRet(self->ll_builder, ret_val);
}

void codegen_block_direct(self_t, MirIndex block_index, LLVMBasicBlockRef ll_block) {
    MirInst *inst = mir_get_inst_tagged(self->mir, block_index, MirBlock);
    MirIndex data_index = inst->data.ty_pl.payload;

    uint32_t stmt_count = mir_get_extra(self->mir, data_index);
    for (uint32_t i = data_index + 1; i <= data_index + stmt_count; i++) {
        LLVMValueRef result = codegen_inst(self, mir_get_extra(self->mir, i), ll_block);
        if (result != NULL) {
            char *value_str = LLVMPrintValueToString(result);
            fprintf(stderr, "Illegal result, expected void but got: %s\n", value_str);
            LLVMDisposeMessage(value_str);
        }
    }

}

#undef self_t