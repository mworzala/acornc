#include <stdlib.h>
#include "codegen.h"

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


void codegen_lower_decl(self_t, Decl *decl) {
    assert(decl != NULL);

    LLVMTypeRef fn_type = codegen_fn_proto(self, decl);
    LLVMValueRef fn = LLVMAddFunction(self->ll_module, decl->name, fn_type);
    self->curr_fn = &fn;

    Mir *mir = decl_get_mir_in_module(decl, self->module);
    self->mir = mir;

    LLVMBasicBlockRef entry_block = LLVMAppendBasicBlock(fn, "entry");
    LLVMPositionBuilderAtEnd(self->ll_builder, entry_block);

    codegen_block_direct(self, 0, entry_block);

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


LLVMValueRef codegen_expr(self_t, MirIndex index, LLVMBasicBlockRef ll_block) {
    MirInst *inst = mir_get_inst(self->mir, index);
    switch (inst->tag) {
        case MirConstant:
            return codegen_constant(self, inst);
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
}

LLVMValueRef codegen_constant(self_t, MirInst *inst) {
    return LLVMConstInt(llvm_int, inst->data.ty_pl.payload, false);
}

void codegen_return(self_t, MirInst *inst, LLVMBasicBlockRef ll_block) {
    //todo codegen for refs
    LLVMValueRef ret_val = codegen_expr(self, ref_to_index(inst->data.un_op), ll_block);
    LLVMBuildRet(self->ll_builder, ret_val);
}

void codegen_block_direct(self_t, MirIndex block_index, LLVMBasicBlockRef ll_block) {
    MirInst *inst = mir_get_inst_tagged(self->mir, block_index, MirBlock);
    MirIndex data_index = inst->data.ty_pl.payload;

    uint32_t stmt_count = mir_get_extra(self->mir, data_index);
    for (uint32_t i = data_index + 1; i <= data_index + stmt_count; i++) {
        LLVMValueRef result = codegen_expr(self, mir_get_extra(self->mir, i), ll_block);
        if (result != NULL) {
            char *value_str = LLVMPrintValueToString(result);
            fprintf(stderr, "Illegal result, expected void but got: %s\n", value_str);
            LLVMDisposeMessage(value_str);
        }
    }

}

#undef self_t