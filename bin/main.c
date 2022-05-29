#include <stdio.h>
#include <stdlib.h>

#include "module.h"

static void run_file(char *path);

int main(int32_t argc, char *argv[]) {

    if (argc == 2) {
        run_file(argv[1]);
    } else {
        fprintf(stderr, "Usage: %s <file>\n", argv[0]);
        exit(64);
    }

    return 0;
}

static void run_file(char *path) {
    Module module;
    module_init(&module, path);

    bool parsed = module_parse(&module);
    if (!parsed) {
        fprintf(stderr, "Could not parse file: %s\n", path);
        exit(64);
    }

    bool lowered = module_lower_main(&module);
    if (!lowered) {
        fprintf(stderr, "Could not lower main for file: %s\n", path);
        exit(64);
    }

    bool emitted = module_emit_llvm(&module);
    if (!emitted) {
        fprintf(stderr, "Could not emit LLVM for file: %s\n", path);
        exit(64);
    }

    module_free(&module);
    exit(0);

//    uint8_t *source = read_file(path);
//
//    Parser parser;
//    parser_init(&parser, source);
//
//    Ast ast = parser_parse(&parser);
//    printf("Done parsing\n");
//
//    char *str = ast_debug_print(&ast);
//    printf("%s\n", str);
//    free(str);
//
//    printf("Generating LLVM IR\n");
//
//    compile_ast_to_file(&ast, "/Users/matt/dev/c/acornc/samples/test.ll");
}

//static uint8_t *read_file(const char *path) {
//    FILE *file = fopen(path, "rb");
//    if (file == NULL) {
//        fprintf(stderr, "Could not open file: %s\n", path);
//        exit(74);
//    }
//
//    fseek(file, 0, SEEK_END);
//    size_t file_size = ftell(file);
//    rewind(file);
//
//    uint8_t *buffer = malloc(file_size + 1);
//    if (buffer == NULL) {
//        fprintf(stderr, "Could not allocate memory for file: %s\n", path);
//        exit(74);
//    }
//
//    size_t bytes_read = fread(buffer, sizeof(uint8_t), file_size, file);
//    if (bytes_read < file_size) {
//        fprintf(stderr, "Could not read file: %s\n", path);
//        exit(74);
//    }
//
//    buffer[bytes_read] = '\0';
//
//    fclose(file);
//    return buffer;
//}
