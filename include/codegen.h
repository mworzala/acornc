#ifndef CONFIG_CODEGEN_H
#define CONFIG_CODEGEN_H

#include "common.h"
#include "llvm-c/Core.h"

// Codegen for a single module

typedef struct module_s Module;

typedef struct codegen_s {
    Module *module;

    LLVMContextRef ll_context;
    LLVMModuleRef ll_module;
    LLVMBuilderRef ll_builder;
} Codegen;

#define self_t Codegen *self

void codegen_init(self_t, Module *module);
void codegen_free(self_t);

bool codegen_write_to_file(self_t, char *path);

#undef self_t

#endif //CONFIG_CODEGEN_H
