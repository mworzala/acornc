#include <stdio.h>
#include <stdlib.h>

#include "lexer.h"
#include "parser.h"

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

//    Lexer lexer;
//    lexer_init(&lexer, source);
//
//    Token token;
//    while ((token = lexer_next(&lexer)).type != TOK_EOF) {
//        printf("%-7s", token_type_to_string(token.type));
//        printf("%lu..%lu ", token.loc.start - start, token.loc.end - start);
//        printf("%.*s", (int)(token.loc.end - token.loc.start), source + (token.loc.start - start));
//        printf("\n");
//    }

    Parser parser;
    parser_init(&parser, source);

    Ast ast = parser_parse(&parser);

    printf("%s\n", source);

    printf("\nIDX |  AST TY | LHS/RHS |  TOK TY | TOK VAL\n");

    for (size_t i = 0; i < ast.nodes.size; i++) {
        AstNode *node = &ast.nodes.data[i];
        printf("%03zu | %7s", i, ast_tag_to_string(node->tag));

        printf(" | ");

        if (node->data.lhs != ast_index_empty) {
            printf("%03u", node->data.lhs);
        } else {
            printf("---");
        }

        printf("/");

        if (node->data.rhs != ast_index_empty) {
            printf("%03u", node->data.rhs);
        } else {
            printf("---");
        }

        printf(" | ");

        Token tok = ast.tokens.data[node->main_token];
        printf("%7s", token_type_to_string(tok.type));

        printf(" | ");

        printf("%.*s", (int)(tok.loc.end - tok.loc.start), source + (tok.loc.start - start));

        printf("\n");
    }

    printf("\n");
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
