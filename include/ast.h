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

#undef self_t

typedef enum ast_tag_s {

    /*
     * main_token:  The token representing the number
     * lhs:         Empty
     * rhs:         Empty
     */
    AST_INTEGER,

    /*
     * main_token: The token representing the operator
     * lhs: The left side expr node
     * rhs: The right side expr node
     */
    AST_BINARY,

    /*
     * All values always empty value.
     */
    AST_EMPTY,
} AstTag;

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
} Ast;

#endif //ACORNC_AST_H
