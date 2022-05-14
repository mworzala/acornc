#include "parse_test_check.h"

extern "C" {
#include "parser.h"
#include "debug/ast_debug.h"
}

testing::AssertionResult parse_check_expr(const char *expr, const char *expected) {
    Parser parser;
    parser_init(&parser, (uint8_t *) expr);

    Ast ast = parser_parse(&parser);
    char *actual = ast_debug_print(&ast);

    bool result = memcmp(actual, expected, strlen(expected)) == 0;
    if (!result) {
        printf("Expected: %s\n", expected);
        printf("Actual: %s\n", actual);
        return testing::AssertionFailure() << "Expected: " << expected << "\nActual: " << actual;
    }

    free(actual);


    return testing::AssertionSuccess();
}
