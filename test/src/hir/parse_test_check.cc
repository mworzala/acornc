#include "parse_test_check.h"

extern "C" {
#include "parser_internal.h"
#include "debug/hir_debug.h"
}

testing::AssertionResult lower_check_generic(LowerFn lower, ParseFn parse, const char *source, const char *expected) {
    // Parse the source.
    Parser parser;
    parser_init(&parser, (uint8_t *) source);

    AstIndex ast_root = parse(&parser);
    Ast ast = (Ast) {
        .source = parser.source,
        .tokens = parser.tokens,
        .nodes = parser.nodes,
        .extra_data = parser.extra_data,
    };

    // Lower the AST to HIR.
    AstLowering lowering;
    ast_lowering_init(&lowering, &ast);

    HirIndex hir_root = lower(&lowering, ast_root);
    Hir hir = (Hir) {
        .instructions = lowering.instructions,
        .extra = lowering.extra,
        .strings = lowering.strings,
    };

    ast_lowering_free(&lowering);

    // Print the HIR to string
    char *actual = hir_debug_print(&hir, hir_root);
    size_t actual_len = strlen(actual);
    // If there are two newlines at the end, remove one of them.
    // Module does this at the moment, should fix it inside module then this is not necessary.
    if (actual[actual_len - 1] == '\n' && actual[actual_len - 2] == '\n') {
        actual[actual_len - 1] = '\0';
        actual_len -= 1;
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
