#ifndef CONFIG_MODULE_H
#define CONFIG_MODULE_H

#include "common.h"
#include "ast.h"
#include "mir.h"
#include "hir.h"
#include "interner.h"
#include "codegen.h"

// SECTION: Declaration

typedef uint32_t DeclIndex;

typedef enum decl_state_s {
    DeclStateUnused,
    DeclStateReferenced,
    DeclStateGenerated,
} DeclState;

typedef enum decl_type_s {
    DECL_TYPE_FN,
    DECL_TYPE_STRUCT,
    DECL_TYPE_ENUM,
} DeclType;

typedef union decl_data_s {
    struct decl_fn_data_s *fn_data;
} DeclData;

typedef struct decl_s {
    StringKey name;
    DeclState state;

    DeclData data;
    Mir *mir; // Only present after lowering
} Decl;

typedef struct decl_fn_data_s {
    Type ret_type;
    uint32_t param_count;
    Type *param_types;
} DeclFnData;



//typedef struct decl_s {
//    char *name;
//    DeclState state;
//
//    AstIndex ast_index;
//
//    // Only present after lowering
//    Mir *mir;
//    // May be present before lowering
//    //todo not good to bring llvm into this struct, will eventually have a Map<decl index, LLVMValueRef>
//    LLVMValueRef llvm_value;
//} Decl;

//#define self_t Decl *self
//
//void decl_init_from_ast(self_t, Ast *ast, AstIndex ast_index);
//void decl_free(self_t);
//
//Mir *decl_get_mir_in_module(self_t, Module *module);
//
//#undef self_t

typedef struct decl_list_s {
    uint32_t size;
    uint32_t capacity;
    Decl *data;
} DeclList;

#define self_t DeclList *self

void decl_list_init(self_t);
void decl_list_free(self_t);
void decl_list_add(self_t, Decl inst);
Decl *decl_list_get(self_t, DeclIndex index);

#undef self_t

// SECTION: Module definition
// A module is a single source file and its associated declarations

typedef struct module_s {
    char *path;
    char *name;

    // Filled during HIR>>MIR lowering
    DeclList decls;

    // Not always present
    Ast *ast;
    // Not always present
    Hir *hir;
    // Only present once codegen has started
    Codegen *codegen;
} Module;

#define self_t Module *self

void module_init(self_t, char *path);
void module_free(self_t);

bool module_parse(self_t);
bool module_lower_ast(self_t);
bool module_lower_main(self_t);
bool module_emit_llvm(self_t);

Decl *module_find_decl(self_t, char *name);

#undef self_t


// Decl

#endif //CONFIG_MODULE_H
