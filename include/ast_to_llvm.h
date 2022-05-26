#ifndef CONFIG_AST_TO_LLVM_H
#define CONFIG_AST_TO_LLVM_H

#include "common.h"
#include "ast.h"

#include <llvm-c/Core.h>

// Public API

void compile_ast_to_file(Ast *ast, const char *filename);

// Private helpers

// ast codegen scope
typedef struct acg_scope_s {
    uint32_t size;
    uint32_t capacity;
    char **names;
    LLVMValueRef *data;
    struct acg_scope_s *parent;
} AcgScope;

#define self_t AcgScope *self

void acg_scope_init(self_t, AcgScope *parent);
void acg_scope_free(self_t);
void acg_scope_set(self_t, const char *name, LLVMValueRef value);
LLVMValueRef *acg_scope_get(self_t, const char *name);

#undef self_t

// ast codegen
typedef struct ast_codegen_s {
    Ast *ast;
    AcgScope root_scope;
    LLVMContextRef context;
    LLVMModuleRef module;
    LLVMBuilderRef builder;
} AstCodeGen;

#define self_t AstCodeGen *self

void ast_codegen_init(self_t, Ast *ast);
void ast_codegen_free(self_t);
void ast_codegen_write(self_t, const char *filename);

void ast_codegen_module(self_t, AstNode *node);

// top level decl
LLVMValueRef ast_codegen_fn_proto(self_t, AstNode *node);
void ast_codegen_named_fn(self_t, AstNode *node);

// statements

// expressions
LLVMValueRef ast_codegen_expr(self_t, AstNode *node, LLVMValueRef fn, AcgScope *scope);
LLVMValueRef ast_codegen_const_int(self_t, AstNode *node);
LLVMValueRef ast_codegen_ref(self_t, AstNode *node, AcgScope *scope);
LLVMValueRef ast_codegen_binary_op(self_t, AstNode *node, LLVMValueRef fn, AcgScope *scope);
// Scope represents the scope for the block, which means the caller should create it.
void ast_codegen_block(self_t, AstNode *node, LLVMValueRef fn, AcgScope *scope);
void ast_codegen_return(self_t, AstNode *node, LLVMValueRef fn, AcgScope *scope);
LLVMValueRef ast_codegen_call(self_t, AstNode *node, LLVMValueRef fn, AcgScope *scope);

// Helpers
AstNode *ast_codegen_node(self_t, AstIndex node_index);
AstNode *ast_codegen_node_from_extra_data(self_t, AstIndex extra_data_index);
// Creates a string of just the content of the token. Must be freed by the caller.
char *ast_codegen_get_token_content(self_t, TokenIndex token_index);

#undef self_t

#endif //CONFIG_AST_TO_LLVM_H
