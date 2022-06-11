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

    int exit_code = execl("/usr/bin/cc", "cc", path, "-lc", "-o", path2, NULL);
    free(path2);

    if (exit_code != 0) {
        printf("cc finished with non-zero exit code: %d\n", exit_code);
        return false;
    }

    return true;
}

static LLVMValueRef codegen_get_decl_ll_value(self_t, Decl *decl) {
    if (decl->llvm_value == NULL) {
        LLVMTypeRef fn_type = codegen_fn_proto(self, decl);
        decl->llvm_value = LLVMAddFunction(self->ll_module, decl->name, fn_type);
        decl->state = DeclStateReferenced;
    }

    return decl->llvm_value;
}

void codegen_lower_decl(self_t, Decl *decl) {
    assert(decl != NULL);

    LLVMValueRef fn = codegen_get_decl_ll_value(self, decl);
    self->curr_fn = &fn;

    if (strcmp(decl->name, "puts") != 0) {
        Mir *mir = decl_get_mir_in_module(decl, self->module);
        self->mir = mir;
        index_ptr_map_init(&self->inst_map);

        LLVMBasicBlockRef entry_block = LLVMAppendBasicBlock(fn, "entry");
        LLVMPositionBuilderAtEnd(self->ll_builder, entry_block);

        codegen_block_direct(self, 0, entry_block);
    }

    decl->state = DeclStateGenerated;

    index_ptr_map_free(&self->inst_map);
    self->mir = NULL;
    self->curr_fn = NULL;
}

// Returns a string containing the content of the token at the given index.
// The caller owns the string memory.
static char *codegen_get_ast_token_content(self_t, TokenIndex token) {
    Token main_token = self->module->ast->tokens.data[token];
    size_t str_len = main_token.loc.end - main_token.loc.start;
    char *str = malloc(str_len + 1);
    memcpy(str, (const void *) main_token.loc.start, str_len);
    str[str_len] = '\0';
    return str;
}

static Type codegen_get_type_from_ast(self_t, AstIndex index) {
    AstNode *node = ast_get_node_tagged(self->module->ast, index, AST_TYPE);

    char *name = codegen_get_ast_token_content(self, node->main_token);
    if (strcmp(name, "*") == 0) {
        free(name);

        // Pointer type
        Type ptr_type = codegen_get_type_from_ast(self, node->data.lhs);

        //todo memory leak, this is never freed. Need to allocate these in an arena probably
        ExtendedType *extended = malloc(sizeof(ExtendedType));
        extended->tag = TY_PTR;
        extended->data.inner_type = ptr_type;

        return (Type) {.extended = extended};
    }

    Type ty = type_from_name(name);
    free(name);
    return ty;
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
        for (int32_t i = 0; i < param_count; i++) {
            AstNode *param_node = ast_get_node_tagged(self->module->ast, self->module->ast->extra_data.data[proto.param_start + i], AST_FN_PARAM);
            Type ty = codegen_get_type_from_ast(self, param_node->data.rhs);
            LLVMTypeRef param_type = codegen_type_to_llvm(self, ty);

            params[i] = param_type;
        }
    }

    // Get return type
    AstNode *ret_ast = ast_get_node_tagged(self->module->ast, proto_ast->data.rhs, AST_TYPE);
    char *type_str = codegen_get_ast_token_content(self, ret_ast->main_token);
    LLVMTypeRef ret_type = codegen_type_to_llvm(self, type_from_name(type_str));
    free(type_str);

    // Create LLVM type
    LLVMTypeRef fn_type = LLVMFunctionType(ret_type, params, param_count, false);

    // Cleanup
    if (params != NULL) {
        free(params);
    }

    return fn_type;
}


LLVMTypeRef codegen_type_to_llvm(self_t, Type type) {
    switch (type_tag(type)) {
        case TypeI8:
            return LLVMInt8TypeInContext(self->ll_context);
        case TypeI16:
            return LLVMInt16TypeInContext(self->ll_context);
        case TypeI32:
            return LLVMInt32TypeInContext(self->ll_context);
        case TypeI64:
            return LLVMInt64TypeInContext(self->ll_context);
        case TypeI128:
            return LLVMInt128TypeInContext(self->ll_context);
        case TY_PTR:
            return LLVMPointerType(codegen_type_to_llvm(self, type.extended->data.inner_type), 0);
        default:
            assert(false);
    }
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
            ll_value = codegen_constant(self, index, ll_block);
            break;
        }
        case MirAdd:
        case MirSub:
        case MirMul:
        case MirDiv:
        case MirEq:
        case MirNEq:
        case MirGt:
        case MirGtEq:
        case MirLt:
        case MirLtEq:
            ll_value = codegen_binary_op(self, index, ll_block);
            break;
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
        case MirCall: {
            ll_value = codegen_call(self, index, ll_block);
            break;
        }
        case MirArg: {
            ll_value = codegen_arg(self, index, ll_block);
            break;
        }
        case MirFnPtr: {
            ll_value = codegen_fn_ptr(self, index);
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

LLVMValueRef codegen_constant(self_t, MirIndex index, LLVMBasicBlockRef ll_block) {
    MirInst *inst = mir_get_inst(self->mir, index);
    Type const_ty = inst->data.ty_pl.ty;

    // Cannot codegen non int/ptr constants
    assert(type_is_integer(const_ty));

    // If it not a pointer, its an int.
    if (type_tag(const_ty) != TY_PTR) {
        LLVMTypeRef ll_type = codegen_type_to_llvm(self, inst->data.ty_pl.ty);
        return LLVMConstInt(ll_type, inst->data.ty_pl.payload, false);
    }

    // It's a pointer, we only support *i8, which means its a const string. for now
    //todo lots of bad assumptions
    assert(type_tag(const_ty.extended->data.inner_type) == TypeI8);

    // Add the string and related instructions
    char *str_content_raw = codegen_get_ast_token_content(self, inst->data.ty_pl.payload);
    char *str_content = malloc(strlen(str_content_raw) - 1);
    strncpy(str_content, str_content_raw + 1, strlen(str_content_raw) - 2);
    str_content[strlen(str_content_raw) - 2] = '\0';
    free(str_content_raw);
    LLVMTypeRef str_type = LLVMArrayType(LLVMInt8Type(), strlen(str_content));
    LLVMValueRef str_global = LLVMAddGlobal(self->ll_module, str_type, "const_string");
    LLVMSetInitializer(str_global, LLVMConstString(str_content, strlen(str_content), false));
    LLVMSetGlobalConstant(str_global, true);
    LLVMSetLinkage(str_global, LLVMPrivateLinkage);
    LLVMSetUnnamedAddress(str_global, LLVMGlobalUnnamedAddr);
    LLVMSetAlignment(str_global, 1);
    free(str_content);

    //todo not sure what below does
    LLVMValueRef zeroIndex = LLVMConstInt( LLVMInt64Type(), 0, true );
    LLVMValueRef indexes[2] = { zeroIndex, zeroIndex };
    LLVMValueRef gep = LLVMBuildInBoundsGEP2(self->ll_builder, str_type, str_global, indexes, 2, "gep");

    return gep;


//todo some yoinked stack overflow code for string literals
//LLVMValueRef defineStringLiteral( const char *sourceString, size_t size ) {
//    LLVMTypeRef strType = LLVMArrayType( LLVMInt8Type(), size );
//    LLVMValueRef str = LLVMAddGlobal(module->getLLVMModule(), strType, "");
//    LLVMSetInitializer(str, LLVMConstString( sourceString, size, true ));
//    LLVMSetGlobalConstant(str, true);
//    LLVMSetLinkage(str, LLVMPrivateLinkage);
//    LLVMSetUnnamedAddress(str, LLVMGlobalUnnamedAddr);
//    LLVMSetAlignment(str, 1);
//
//
//    LLVMValueRef zeroIndex = LLVMConstInt( LLVMInt64Type(), 0, true );
//    LLVMValueRef indexes[2] = { zeroIndex, zeroIndex };
//
//    LLVMValueRef gep = LLVMBuildInBoundsGEP2(builder, strType, str, indexes, 2, "");
//
//    return gep;
//}


    assert(false);
}

LLVMValueRef codegen_binary_op(self_t, MirIndex index, LLVMBasicBlockRef ll_block) {
    MirInst *inst = mir_get_inst(self->mir, index);

    LLVMValueRef lhs = codegen_inst(self, ref_to_index(inst->data.bin_op.lhs), ll_block);
    LLVMValueRef rhs = codegen_inst(self, ref_to_index(inst->data.bin_op.rhs), ll_block);

    if (inst->tag == MirAdd) {
        return LLVMBuildAdd(self->ll_builder, lhs, rhs, "add");
    } else if (inst->tag == MirSub) {
        return LLVMBuildSub(self->ll_builder, lhs, rhs, "sub");
    } else if (inst->tag == MirMul) {
        return LLVMBuildMul(self->ll_builder, lhs, rhs, "mul");
    } else if (inst->tag == MirDiv) {
        return LLVMBuildSDiv(self->ll_builder, lhs, rhs, "div");
    } else {
        fprintf(stderr, "Unhandled binary op: %s\n", mir_tag_to_string(inst->tag));
        assert(false);
    }
}

LLVMValueRef codegen_alloc(self_t, MirIndex index, LLVMBasicBlockRef ll_block) {
    MirInst *inst = mir_get_inst_tagged(self->mir, index, MirAlloc);

    LLVMTypeRef type = codegen_type_to_llvm(self, inst->data.ty);

    return LLVMBuildAlloca(self->ll_builder, type, "alloc"); //todo preserve name somehow
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

LLVMValueRef codegen_call(self_t, MirIndex index, LLVMBasicBlockRef ll_block) {
    MirInst *inst = mir_get_inst_tagged(self->mir, index, MirCall);

    // Get operand ir value
    LLVMValueRef fn_ptr = codegen_inst(self, ref_to_index(inst->data.pl_op.operand), ll_block);
    bool is_a_fn = LLVMIsAFunction(fn_ptr);
    if (!is_a_fn) {
        char *value_str = LLVMPrintValueToString(fn_ptr);
        fprintf(stderr, "Expected function pointer, got %s\n", value_str);
        LLVMDisposeMessage(value_str);
        assert(false);
    }

    // Construct arg list
    MirIndex extra_index = inst->data.pl_op.payload;

    uint32_t arg_count = mir_get_extra(self->mir, extra_index);
    LLVMValueRef *args = NULL;
    if (arg_count != 0) {
        args = malloc(sizeof(LLVMValueRef) * arg_count);
        for (uint32_t i = 0; i < arg_count; i++) {
            args[i] = codegen_inst(self, ref_to_index(mir_get_extra(self->mir, extra_index + i + 1)), ll_block);
        }
    }

    // Build the call instruction
    LLVMValueRef call_ir = LLVMBuildCall(self->ll_builder, fn_ptr, args, arg_count, "call");

    if (args != NULL)
        free(args);
    return call_ir;
}

LLVMValueRef codegen_arg(self_t, MirIndex index, LLVMBasicBlockRef ll_block) {
    MirInst *inst = mir_get_inst_tagged(self->mir, index, MirArg);

    return LLVMGetParam(*self->curr_fn, inst->data.ty_pl.payload);
}

LLVMValueRef codegen_fn_ptr(self_t, MirIndex index) {
    MirInst *inst = mir_get_inst_tagged(self->mir, index, MirFnPtr);

    char *fn_name = inst->data.fn_ptr;
    Decl *decl = module_find_decl(self->module, fn_name);
    if (decl == NULL) {
        fprintf(stderr, "Could not find function %s\n", fn_name);
        assert(false);
    }

    return codegen_get_decl_ll_value(self, decl);
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