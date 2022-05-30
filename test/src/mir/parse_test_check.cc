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
    AstNode *ast_module = ast_get_node_tagged(&ast, ast_index_root, AST_MODULE);
    assert(ast_module->data.lhs != ast_index_empty); // Ensure there is at least one function


    char *actual = static_cast<char *>(malloc(1024 * 16));
    memset(actual, 0, 1024 * 16);

    if (!extended) {
        AstIndex idx = ast.extra_data.data[ast_module->data.lhs];

        AstToMir lower;
        ast_to_mir_init(&lower, &ast);
        Mir mir = lower_ast_fn(&lower, idx);

        char *mir_str = mir_debug_print(&mir);
        sprintf(actual + strlen(actual), "%s", mir_str);
        free(mir_str);
    } else {
        for (AstIndex index = ast_module->data.lhs; index <= ast_module->data.rhs; index++) {
            AstIndex idx = ast.extra_data.data[index];

            AstNode *fn_node = ast_get_node(&ast, idx);
            Token main_token = ast.tokens.data[fn_node->main_token + 1];
            size_t str_len = main_token.loc.end - main_token.loc.start;
            char *str = static_cast<char *>(malloc(str_len + 1));
            memcpy(str, (const void *) main_token.loc.start, str_len);
            str[str_len] = '\0';

            sprintf(actual + strlen(actual), "// begin fn %s\n", str);

            AstToMir lower;
            ast_to_mir_init(&lower, &ast);
            Mir mir = lower_ast_fn(&lower, idx);

            char *mir_str = mir_debug_print(&mir);
            sprintf(actual + strlen(actual), "%s", mir_str);
            free(mir_str);

            sprintf(actual + strlen(actual) - 1, "// end fn %s\n\n", str);
            free(str);
        }
    }

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
        free(actual);
        return testing::AssertionFailure() << "Expected: " << expected << "\nActual: " << actual;
    }

    free(actual);
    return testing::AssertionSuccess();
}
