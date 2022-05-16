#ifndef ACORNC_AST_H
#define ACORNC_AST_H

#include "common.h"
#include "lexer.h"

// Represents an index into the token array for the contained file.
//todo maybe move me to lexer file
typedef uint32_t TokenIndex;

// Represents an index into the ast node list (of an AstNode)
typedef uint32_t AstIndex;

#define ast_index_empty ((AstIndex) UINT32_MAX)

typedef struct ast_index_list_s {
    uint32_t size;
    uint32_t capacity;
    AstIndex *data;
} AstIndexList;

#define self_t AstIndexList *self

void ast_index_list_init(self_t);
void ast_index_list_free(self_t);
void ast_index_list_add(self_t, AstIndex index);
void ast_index_list_add_multi(self_t, void *data, size_t size);

#define ast_index_list_add_sized(self, data) ast_index_list_add_multi(self, &data, sizeof(data) / sizeof(AstIndex))

#undef self_t

//todo tests to verify that all required changes have been made when updating this list (tag to string, ast_debug)
typedef enum ast_tag_s {

    // Expressions

    // main_token:  The token representing the integer literal
    AST_INTEGER,

    // main_token:  The token representing the bool literal
    AST_BOOL,

    // main_token:  The token representing the reference identifier
    AST_REF,

    // main_token:  The token representing the operator
    // lhs:         The left side expr node
    // rhs:         The right side expr node
    AST_BINARY,

    // main_token:  The token representing the operator
    // lhs:         The expr being operated on
    // rhs:         Empty
    AST_UNARY,

    // main_token:  The token representing the '{'
    // lhs..rhs:    First..last contained stmt
    //              Both empty if block is empty
    AST_BLOCK,

    // main_token:  The token representing 'return'
    // lhs/rhs:     The expr being returned, if present/Empty
    AST_RETURN,

    // main_token:  The token representing 'if'
    // lhs/rhs   :  Condition expr/IfData
    AST_IF,

    // main_token:  The token representing the 'while'
    // lhs/rhs   :  Condition expr/Body block
    AST_WHILE,

    // main_token:  The token representing the '.'
    // lhs/rhs   :  LHS expr/RHS expr (identifier
    AST_DOT, //todo what is this usually called? access?

    // main_token:  The token representing '('
    // lhs/rhs   :  The expr being called/AstCallData (see below)
    AST_CALL,

    // Statements

    // main_token:  The token representing the `let` keyword
    //              The identifier always follows this token
    // lhs:         The type expression, if present todo this is never present for now.
    // rhs:         The initializer expression, if present
    AST_LET,

    // Top level decls

    // main_token:  The token representing the `fn` keyword
    // lhs/rhs:     Prototype/Body
    AST_NAMED_FN,

    // main_token:  The token representing the `struct` keyword
    // lhs..rhs  :  The field entries
    AST_STRUCT,

    // main_token:  The token representing the `enum` keyword
    // lhs..rhs  :  The variant cases
    AST_ENUM,

    // Special

    // main_token:  The token representing the function name
    // lhs/rhs   :  AstFnProto (see below) within extra_data/Type expr
    //todo this eventually will be changed since it will be anonymous/not know the function name.
    AST_FN_PROTO,

    // main_token:  The token representing the identifier
    // lhs/rhs   :  Unused/Type expr
    // Note: Type expr is _not_ required in the AST. It will be an error during lowering if it is not present.
    //       todo it actually might end up as a parse error and be required in later phases
    //todo lhs will eventually be used to store flags like is_mut, is_ref, etc.
    AST_FN_PARAM,

    // main_token:  The token representing the identifier
    // lhs/rhs   :  Unused/Type expr
    //todo lhs will eventually be used to store flags like is_ref, etc.
    AST_FIELD,

    // main_token:  The token representing the identifier
    // lhs/rhs   :  Unused/Unused
    AST_ENUM_CASE,

    // main_token:  Unused
    // lhs..rhs  :  first..last top level decl
    AST_MODULE,

    // All values always empty value.
    AST_EMPTY,

    __AST_LAST, //todo test case to ensure each one is stringified
} AstTag;

typedef struct ast_fn_proto_s {
    AstIndex param_start;
    AstIndex param_end;
} AstFnProto;

typedef struct ast_call_data_s {
    AstIndex arg_start;
    AstIndex arg_end;
} AstCallData;

typedef struct ast_if_data_s {
    AstIndex then_block;
    AstIndex else_block;
} AstIfData;

const char *ast_tag_to_string(AstTag tag);

typedef struct ast_data_s {
    AstIndex lhs;
    AstIndex rhs;
} AstData;

typedef struct ast_node_s {
    AstTag tag;
    TokenIndex main_token;
    AstData data;
} AstNode;

//todo Zig uses node position zero to represent the root, which cannot be referenced by another node,
//     meaning that index value zero can be used to represent null.
//     Could also get rid of "root" in Ast.
typedef struct ast_node_list_s {
    uint32_t size;
    uint32_t capacity;
    AstNode *data;
} AstNodeList;

#define self_t AstNodeList *self

void ast_node_list_init(self_t);
void ast_node_list_free(self_t);
void ast_node_list_add(self_t, AstNode node);

#undef self_t

typedef struct ast_s {
    uint8_t *source;
    TokenList tokens;

    AstNodeList nodes;
    AstIndexList extra_data;
    AstIndex root;
} Ast;

#endif //ACORNC_AST_H
