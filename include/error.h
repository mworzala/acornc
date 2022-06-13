#ifndef ACORN_ERROR_H
#define ACORN_ERROR_H

#include "common.h"

// Represents a position in a source file.
typedef uint32_t Loc;

// Represents a start and end position in a source file.
typedef struct span_s {
    Loc start;
    Loc end;
} Span;

// A generic error for use throughout the compiler.
// Field meaning depends on the context of the error.
typedef struct compile_error_s {
    // The location where the error occurred. End position may be missing if it is a single position.
    Span location;
    // A relevant node index, for example a token in the token stream, an ast node, etc.
    // Content depends on the context of the error, and may be zero.
    uint32_t node;

    // Error code, depends on context of error.
    uint32_t error_code;
    // Raw string content of the message, will use data below in the future
    // Memory is owned by the error.
    char *message;

    // Replacements for error messages, content depends completely on the type of error.
    //todo not used for now
//    void **data;  //todo not sure the semantics of this yet. Might replace with struct
} CompileError;

// SECTION: Error list
// Contains a list of errors owned by the list.

typedef struct error_list_s {
    uint32_t size;
    uint32_t capacity;
    CompileError **data;
} ErrorList;

#define self_t ErrorList *self

void error_list_init(self_t);
void error_list_free(self_t);
// Add an error, takes ownership of the data within the error.
void error_list_add(self_t, CompileError error);
CompileError *error_list_get(self_t, uint32_t index);

#undef self_t

#endif //ACORN_ERROR_H
