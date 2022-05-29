#include "module.h"

#include <string.h>
#include <stdlib.h>

#include "array_util.h"
#include "parser.h"
#include "ast_to_mir.h"

// SECTION: Declaration

#define self_t Decl *self

void decl_init(self_t, char *name) {
    self->name = name;
    self->mir = NULL;
}

void decl_free(self_t) {
    free(self->name);
    self->name = NULL;
//    mir_free(self->mir); //todo
    self->mir = NULL;
}

#undef self_t

#define self_t DeclList *self

void decl_list_init(self_t) {
    self->size = 0;
    self->capacity = 0;
    self->data = NULL;
}

void decl_list_free(self_t) {
    ARRAY_FREE(Decl, self->data);
    decl_list_init(self);
}

void decl_list_add(self_t, Decl inst) {

    if (self->capacity < self->size + 1) {
        self->capacity = ARRAY_GROW_CAPCITY(self->capacity);
        self->data = ARRAY_GROW(Decl, self->data, self->capacity);
    }

    self->data[self->size] = inst;
    self->size++;
}

Decl *decl_list_get(self_t, DeclIndex index) {
    if (index < self->size)
        return &self->data[index];
    return NULL;
}

#undef self_t


// SECTION: Module definition

#define self_t Module *self

void module_init(self_t, char *path) {
    self->path = path;
    self->name = strrchr(path, '/') + 1;

    self->ast = NULL;
    decl_list_init(&self->decls);
    self->codegen = NULL;
}

void module_free(self_t) {
    self->path = NULL;
    self->name = NULL;

    if (self->ast) {
//        ast_free(self->ast); //todo
        self->ast = NULL;
    }
    decl_list_free(&self->decls);
    if (self->codegen) {
        codegen_free(self->codegen);
        self->codegen = NULL;
    }
}


static uint8_t *read_file(const char *path) {
    FILE *file = fopen(path, "rb");
    if (file == NULL) {
        fprintf(stderr, "Could not open file: %s\n", path);
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    size_t file_size = ftell(file);
    rewind(file);

    uint8_t *buffer = malloc(file_size + 1); //todo must be freed in all cases
    if (buffer == NULL) {
        fprintf(stderr, "Could not allocate memory for file: %s\n", path);
        return NULL;
    }

    size_t bytes_read = fread(buffer, sizeof(uint8_t), file_size, file);
    if (bytes_read < file_size) {
        fprintf(stderr, "Could not read file: %s\n", path);
        return NULL;
    }

    buffer[bytes_read] = '\0';

    fclose(file);
    return buffer;
}


bool module_parse(self_t) {
    assert(self->ast == NULL);

    // Read source
    // `source` ownership is given to the ast.
    uint8_t *source = read_file(self->path);
    if (source == NULL) {
        return false;
    }

    // Lex
    Parser parser;
    parser_init(&parser, source);

    // Parse
    *self->ast = parser_parse(&parser);
    return true;
}

bool module_lower_main(self_t) {
    assert(self->ast != NULL);

    // Lower
    return true;
}

bool module_emit_llvm(self_t) {
    assert(self->codegen != NULL);

    char *llvm_path = malloc(strlen(self->path) + 3);
    strcpy(llvm_path, self->path);
    strcat(llvm_path, ".ll");

    return codegen_write_to_file(self->codegen, llvm_path);
}

#undef self_t


