#include "ast_err_reporter.h"

#include "color.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static uint64_t get_line_col(uint8_t *str, uint32_t pos) {
    uint32_t row = 1, col = 0;
    for (uint32_t i = 0; i < pos; i++) {
        uint8_t c = str[i];
        if (c == '\0') assert(false);

        if (c == '\n') {
            row++;
            col = 0;
        } else {
            col++;
        }
    }
    return (uint64_t) row << 32 | col;
}

static char *get_line(uint8_t *str, uint32_t line) {
    // Copilot wrote this, not me :|
    uint32_t start = 0, end = 0;

    // Find the start of the line
    for (uint32_t i = 0; i < strlen((char *) str); i++) {
        uint8_t c = str[i];
        if (c == '\0') assert(false);

        if (c == '\n') {
            if (line == 1) {
                start = i + 1;
                break;
            }
            line--;
        }
    }

    // Find the end of the line
    for (uint32_t i = start; i < strlen((char *) str); i++) {
        uint8_t c = str[i];
        if (c == '\0') assert(false);

        if (c == '\n') {
            end = i;
            break;
        }
    }

    // Copy the line
    char *line_str = (char *) malloc(end - start + 1);
    memcpy(line_str, str + start, end - start);
    line_str[end - start] = '\0';
    return line_str;
}

static void print_ast_error(char *file_name, Ast *ast, CompileError *error) {
    if (error->location.start == UINT32_MAX) {
        printf("An error occurred but was incomplete.\n");
        return;
    }

    uint64_t row_col = get_line_col(ast->source, error->location.start);
    uint32_t row = row_col >> 32;
    uint32_t col = row_col & 0xFFFFFFFF;

    char *line = get_line(ast->source, row - 1);

    char *error_string = ast_error_to_string(error->error_code);
    char *full_error_string = "Expected expression, found ';'";

    printf("%s:%d:%d: " RED "error" reset ": %s\n", file_name, row, col, error_string);
    printf("%s\n", line);
    printf("%*s" GRN "^" reset "\n", col, "");

}

void print_ast_errors(char *file_name, Ast *ast) {
    for (int i = 0; i < ast->errors.size; i++) {
        CompileError *error = ast->errors.data[i];
        print_ast_error(file_name, ast, error);
    }
}
