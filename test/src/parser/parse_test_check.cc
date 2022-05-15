#include "parse_test_check.h"

extern "C" {
#include "parser.h"
#include "parser_internal.h"
#include "debug/ast_debug.h"
}

testing::AssertionResult parse_check_expr(const char *expr, const char *expected) {
    Parser parser;
    parser_init(&parser, (uint8_t *) expr);

    AstIndex root = int_expr(&parser);
    Ast ast = (Ast) {
        .source = parser.source,
        .tokens = parser.tokens,
        .nodes = parser.nodes,
        .extra_data = parser.extra_data,
        .root = root,
    };
    char *actual = ast_debug_print(&ast);

    bool result = memcmp(actual, expected, strlen(expected)) == 0;
    if (!result) {
        printf("Expected:\n%s\n", expected);
        printf("Actual:\n%s\n", actual);
        return testing::AssertionFailure() << "Expected: " << expected << "\nActual: " << actual;
    }

    free(actual);


    return testing::AssertionSuccess();
}
