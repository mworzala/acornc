#include "codegen.h"

#include "module.h"

#define self_t Codegen *self

void codegen_init(self_t, Module *module) {
    assert(module != NULL);
    self->module = module;

    self->ll_context = LLVMContextCreate();
    self->ll_module = LLVMModuleCreateWithNameInContext(module->name, self->ll_context);
    self->ll_builder = LLVMCreateBuilderInContext(self->ll_context);
}

void codegen_free(self_t) {
    self->module = NULL;

    LLVMContextDispose(self->ll_context);
    LLVMDisposeModule(self->ll_module);
    LLVMDisposeBuilder(self->ll_builder);
}

bool codegen_write_to_file(self_t, char *path) {
    char *error = NULL;
    bool result = LLVMPrintModuleToFile(self->ll_module, path, &error);
    if (!result) {
        fprintf(stderr, "Error writing to file: %s\n", error);
        LLVMDisposeMessage(error);
    }
    return result;
}

#undef self_t