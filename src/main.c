#include <stdio.h>
#include <stdlib.h>

#include "lexer.h"

static uint8_t *read_file(const char *path);
static void run_file(const char *path);

int main(int32_t argc, char *argv[]) {

    if (argc == 2) {
        run_file(argv[1]);
    } else {
        fprintf(stderr, "Usage: %s <file>\n", argv[0]);
        exit(64);
    }

    return 0;
}

static void run_file(const char *path) {
    uint8_t *source = read_file(path);
    size_t start = (size_t) source;

    Lexer lexer;
    lexer_init(&lexer, source);

    Token token;
    while ((token = lexer_next(&lexer)).type != TOK_EOF) {
        printf("%-7s", token_type_to_string(token.type));
        printf("%lu..%lu ", token.loc.start - start, token.loc.end - start);
        printf("%.*s\n", (int)(token.loc.end - token.loc.start), source + (token.loc.start - start));
        printf("\n");
    }
}

static uint8_t *read_file(const char *path) {
    FILE *file = fopen(path, "rb");
    if (file == NULL) {
        fprintf(stderr, "Could not open file: %s\n", path);
        exit(74);
    }

    fseek(file, 0, SEEK_END);
    size_t file_size = ftell(file);
    rewind(file);

    uint8_t *buffer = malloc(file_size + 1);
    if (buffer == NULL) {
        fprintf(stderr, "Could not allocate memory for file: %s\n", path);
        exit(74);
    }

    size_t bytes_read = fread(buffer, sizeof(uint8_t), file_size, file);
    if (bytes_read < file_size) {
        fprintf(stderr, "Could not read file: %s\n", path);
        exit(74);
    }

    buffer[bytes_read] = '\0';

    fclose(file);
    return buffer;
}
