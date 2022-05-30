#include "parse_test_check.h"

extern "C" {
#include "parser.h"
#include "ast_to_mir.h"
#include "debug/mir_debug.h"
}

testing::AssertionResult parse_check_mir(bool extended, const char *expr, const char *expected) {
    Parser parser;
    parser_init(&parser, (uint8_t *) expr);

    Ast ast = parser_parse(&parser);
    // Extract first function index
    //todo make this smarter / output all mir
    AstNode *ast_module = ast_get_node_tagged(&ast, ast.root, AST_MODULE);
    assert(ast_module->data.lhs != ast_index_empty); // Ensure there is at least one function
    //todo print all function airs
    AstIndex idx = ast.extra_data.data[ast_module->data.lhs];

    AstToMir lower;
    ast_to_mir_init(&lower, &ast);
    Mir mir = lower_ast_fn(&lower, idx);

    char *actual = mir_debug_print(&mir);
    size_t actual_len = strlen(actual);
    // If there are two newlines at the end, remove one of them.
    // Module does this at the moment, should fix it inside module then this is not necessary.
    if (actual[actual_len - 1] == '\n' && actual[actual_len - 2] == '\n') {
        actual[actual_len - 1] = '\0';
        actual_len -= 1;
    }

    bool result = actual_len == strlen(expected) && strcmp(actual, expected) == 0;
    if (!result) {
        printf("Expected:\n%s\n", expected);
        printf("Actual:\n%s\n", actual);
        return testing::AssertionFailure() << "Expected: " << expected << "\nActual: " << actual;
    }

    free(actual);


    return testing::AssertionSuccess();
}
