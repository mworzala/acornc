#ifndef ACORNC_PARSER_INTERNAL_H
#define ACORNC_PARSER_INTERNAL_H

#include "common.h"
#include "lexer.h"
#include "ast.h"

typedef struct parser_s Parser;

#define self_t Parser *self

// Parser utilities
Token parse_peek_last(self_t);
Token parse_peek_curr(self_t);
Token parse_advance(self_t);

bool parse_match(self_t, TokenType type);

// Returns true and advances if the next token is the expected token
// Otherwise returns false and does not advance
bool parse_match_advance(self_t, TokenType type);

// Assert that the next token is of the given type, or panic
//todo very unsafe mechanic, should enter recovery mode or at least skip until it finds that token
TokenIndex parse_assert(self_t, TokenType type);


// Pratt BP
typedef struct binding_power_s {
    uint8_t lhs;
    uint8_t rhs;
} BindingPower;

BindingPower token_bp(Token token, bool is_prefix);

// Top level
AstIndex int_module(self_t);
AstIndex int_top_level_decl(self_t);
AstIndex tl_const_decl(self_t);
AstIndex tl_fn_decl(self_t);
AstIndex tl_struct_decl(self_t);
AstIndex tl_enum_decl(self_t);


// Statements
AstIndex int_stmt(self_t);
AstIndex stmt_let(self_t);


// Expressions
AstIndex int_expr(self_t);
AstIndex int_expr_bp(self_t);
// Return a literal ast node or `empty_ast_index` if the next token is not a literal.
AstIndex expr_literal(self_t);
AstIndex expr_block(self_t);
AstIndex expr_return(self_t);
AstIndex expr_if(self_t);
AstIndex expr_while(self_t);


// Type expressions
AstIndex type_expr_constant(self_t);


// Special
AstIndex fn_proto(self_t, bool foreign);
AstIndex fn_param(self_t);
AstIndex struct_field(self_t);
AstIndex enum_case(self_t);
// Returns an extra_data index for an AstCallData
AstIndex call_data(self_t);

// Unmapped
typedef struct ast_index_pair_s {
    AstIndex first;
    AstIndex second;
} AstIndexPair;

// Parses a list of expressions, think function parameters
// parse_fn=fn_param open=( close=) delimiter=,
// Has language semantics of allowing a trailing delimiter or not.
AstIndexPair int_parse_list(self_t, AstIndex (*parse_fn)(self_t), TokenType open, TokenType close, TokenType delimiter);

#undef self_t

typedef struct parse_frame_s {
    uint8_t min_bp;
    AstIndex lhs;
    TokenIndex op_idx;
} ParseFrame;

typedef struct parse_frame_stack_s {
    uint32_t size;
    uint32_t capacity;
    ParseFrame *data;
} ParseFrameStack;

#define self_t ParseFrameStack *self

void parse_frame_stack_init(self_t);
void parse_frame_stack_free(self_t);
void parse_frame_stack_push(self_t, ParseFrame frame);
ParseFrame parse_frame_stack_pop(self_t);
bool parse_frame_stack_empty(self_t);

#undef self_t

#endif //ACORNC_PARSER_INTERNAL_H
