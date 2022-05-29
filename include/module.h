#ifndef CONFIG_MODULE_H
#define CONFIG_MODULE_H

#include "common.h"
#include "ast.h"
#include "mir.h"
#include "codegen.h"

// SECTION: Declaration

typedef uint32_t DeclIndex;

typedef struct decl_s {
    char *name;

    AstIndex ast_index;

    // Only present after lowering
    Mir *mir;
} Decl;

#define self_t Decl *self

void decl_init_from_ast(self_t, Ast *ast, AstIndex ast_index);
void decl_free(self_t);

Mir *decl_get_mir_in_module(self_t, Module *module);

#undef self_t

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

    // Not always present
    Ast *ast;
    // Filled late
    DeclList decls;
    // Only present once codegen has started
    Codegen *codegen;

} Module;

#define self_t Module *self

void module_init(self_t, char *path);
void module_free(self_t);

bool module_parse(self_t);
bool module_lower_main(self_t);
bool module_emit_llvm(self_t);

Decl *module_find_decl(self_t, char *name);

#undef self_t


// Decl

#endif //CONFIG_MODULE_H
