#include "parse_test_check.h"

extern "C" {
#include "debug/ast_debug_tree.h"
}

testing::AssertionResult parse_check_generic(ParseFn parse, const char *expr, const char *expected, bool print_locs) {
    Parser parser;
    parser_init(&parser, (uint8_t *) expr);

    AstIndex root = parse(&parser);
    Ast ast = (Ast) {
        .source = parser.source,
        .tokens = parser.tokens,
        .nodes = parser.nodes,
        .extra_data = parser.extra_data,
    };
    char *actual = ast_debug_tree_print(&ast, root, print_locs);
    size_t actual_len = strlen(actual);
    // If there are two newlines at the end, remove one of them.
    // Module does this at the moment, should fix it inside module then this is not necessary.
    if (actual[actual_len - 1] == '\n' && actual[actual_len - 2] == '\n') {
        actual[actual_len - 1] = '\0';
        actual_len -= 1;
    }

    if (parser.errors.size > 0) {
        for (size_t i = 0; i < parser.errors.size; i++) {
            CompileError *err = parser.errors.data[i];
            sprintf(actual + strlen(actual), "ERR : %s", ast_error_to_string(static_cast<AstError>(err->error_code)));

            Span span = err->location;
            if (span.start != UINT32_MAX) {
                sprintf(actual + strlen(actual), " @ %d", span.start);

                if (span.end != UINT32_MAX) {
                    sprintf(actual + strlen(actual), "..%d", span.end);
                }
            }

            sprintf(actual + strlen(actual), "\n");
        }
    }

    actual_len = strlen(actual);

    bool result = actual_len == strlen(expected) && strcmp(actual, expected) == 0;
    if (!result) {
        printf("Expected:\n%s\n", expected);
        printf("Actual:\n%s\n", actual);
        return testing::AssertionFailure() << "Expected: " << expected << "\nActual: " << actual;
    }

    free(actual);


    return testing::AssertionSuccess();
}
