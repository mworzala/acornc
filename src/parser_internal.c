#include "parser_internal.h"

#include <assert.h>

#include "array_util.h"
#include "parser.h"

#define self_t Parser *self

// SECTION: Parsing utilities

Token parse_peek_curr(self_t) {
    return self->tokens.data[self->tok_index];
}

Token parse_advance(self_t) {
    if (self->tokens.size <= self->tok_index + 1) {
        // Always return last element, which is known to be EOF.
        return self->tokens.data[self->tokens.size - 1];
    }

    Token tok = parse_peek_curr(self);
    self->tok_index++;
    return tok;
}

bool parse_match(self_t, TokenType type) {
    return parse_peek_curr(self).type == type;
}

bool parse_match_advance(self_t, TokenType type) {
    Token tok = parse_peek_curr(self);
    if (tok.type != type) {
        return false;
    }

    parse_advance(self);
    return true;
}

TokenIndex parse_assert(self_t, TokenType type) {
    Token tok = parse_advance(self);
    if (tok.type != type) {
        printf("Expected token of type %s, got %s\n", token_type_to_string(type), token_type_to_string(tok.type));
        assert(false);
    }
    return self->tok_index - 1;
}

static AstIndex error(self_t, AstError code) {
    Token current_tok = parse_peek_curr(self);
    error_list_add(&self->errors, (CompileError) {
        .error_code = code,
        .node = ast_index_empty,
        .location = {current_tok.loc.start, UINT32_MAX},
        .data = NULL,
    });
    ast_node_list_add(&self->nodes, (AstNode) {
        .tag = AST_ERROR,
        .main_token = UINT32_MAX,
        .data = { ast_index_empty, ast_index_empty },
    });
    return self->nodes.size - 1;
}

AstIndex parse_error(self_t) {
    if (parse_peek_curr(self).type == TOK_EOF) {
        return error(self, AST_ERR_UNEXPECTED_EOF);
    }
    assert(false);
}


// SECTION: Parsing implementation

//region Top level declarations

AstIndex int_module(self_t) {
    // See int_parse_list, it is a better documented, similar version of this logic.

    // Allocate an empty ast node immediately so that the module node can fill it at index zero
    assert(self->nodes.size == 0);
    ast_node_list_add(&self->nodes, (AstNode) {
        .tag = AST_MODULE,
        .main_token = UINT32_MAX,
        .data = {ast_index_empty, ast_index_empty},
    });

    // Parse the module
    IndexList inner_indices;
    index_list_init(&inner_indices);

    // Parse inner expressions
    while (!parse_match(self, TOK_EOF)) {
        AstIndex idx = int_top_level_decl(self);
        index_list_add(&inner_indices, idx);

        if (idx == ast_index_empty) {
            assert(false);
        }
    }

    // First node
    AstIndex start = ast_index_empty;
    AstIndex end = ast_index_empty;

    if (inner_indices.size != 0) {
        start = self->extra_data.size;
        end = self->extra_data.size + inner_indices.size - 1;

        // Copy inner_indices to extra_data
        for (size_t i = 0; i < inner_indices.size; i++) {
            index_list_add(&self->extra_data, inner_indices.data[i]);
        }
    }
    index_list_free(&inner_indices);

    // Update the module node
    AstNode *module_node = &self->nodes.data[0];
    module_node->data = (AstData) {start, end};
    return ast_index_root;
}

AstIndex int_top_level_decl(self_t) {
    if (parse_match(self, TOK_FN) || parse_match(self, TOK_FOREIGN)) {
        return tl_fn_decl(self);
    } else if (parse_match(self, TOK_STRUCT)) {
        return tl_struct_decl(self);
    } else if (parse_match(self, TOK_ENUM)) {
        return tl_enum_decl(self);
    }

    return ast_index_empty;
}

AstIndex tl_fn_decl(self_t) {
    bool foreign = parse_match_advance(self, TOK_FOREIGN);

    TokenIndex main_token = parse_assert(self, TOK_FN);

    // Parse prototype
    AstIndex prototype = fn_proto(self, foreign);

    // Parse body
    AstIndex body_block = ast_index_empty;
    if (foreign) {
        parse_assert(self, TOK_SEMI);
    } else {
        body_block = expr_block(self);
    }

    // Construct node
    ast_node_list_add(&self->nodes, (AstNode) {
        .tag = AST_NAMED_FN,
        .main_token = main_token,
        .data = {
            .lhs = prototype,
            .rhs = body_block,
        }});
    return self->nodes.size - 1;
}

AstIndex tl_struct_decl(self_t) {
    TokenIndex main_token = parse_assert(self, TOK_STRUCT);

    parse_assert(self, TOK_IDENT);// Eat the identifier, can be accessed using main_token + 1

    AstIndexPair entry_data = int_parse_list(self, struct_field,
                                             TOK_LBRACE, TOK_RBRACE, TOK_SEMI);

    ast_node_list_add(&self->nodes, (AstNode) {
        .tag = AST_STRUCT,
        .main_token = main_token,
        .data = {entry_data.first, entry_data.second}});
    return self->nodes.size - 1;
}

AstIndex tl_enum_decl(self_t) {
    TokenIndex main_token = parse_assert(self, TOK_ENUM);

    parse_assert(self, TOK_IDENT);// Eat the identifier, can be accessed using main_token + 1

    AstIndexPair entry_data = int_parse_list(self, enum_case,
                                             TOK_LBRACE, TOK_RBRACE, TOK_COMMA);

    ast_node_list_add(&self->nodes, (AstNode) {
        .tag = AST_ENUM,
        .main_token = main_token,
        .data = {entry_data.first, entry_data.second}});
    return self->nodes.size - 1;
}

//endregion

//region Statements

AstIndex int_stmt(self_t) {
    if (parse_peek_curr(self).type == TOK_LET) {
        return stmt_let(self);
    }

    return int_expr(self);
}

AstIndex stmt_let(self_t) {
    TokenIndex main_token = parse_assert(self, TOK_LET);

    // Ensure identifier is present.
    // The token stream stays around in the next phase, so this may be fetched in the future.
    parse_assert(self, TOK_IDENT);

    // Parse the type expression, if present
    AstIndex type_expr = ast_index_empty;
    if (parse_match_advance(self, TOK_COLON)) {
        type_expr = type_expr_constant(self);
    }

    // Parse the initializer, if present
    AstIndex init_expr = ast_index_empty;
    if (parse_match_advance(self, TOK_EQ)) {
        init_expr = int_expr(self);
    }

    // Generate the AST node itself.
    ast_node_list_add(&self->nodes, (AstNode) {
        .tag = AST_LET,
        .main_token = main_token,
        .data = {
            .lhs = type_expr,
            .rhs = init_expr,
        },
    });
    return self->nodes.size - 1;
}

//endregion

//region Expressions

AstIndex int_expr(self_t) {
    if (parse_match(self, TOK_LBRACE)) {
        return expr_block(self);
    } else if (parse_match(self, TOK_RETURN)) {
        return expr_return(self);
    } else if (parse_match(self, TOK_IF)) {
        return expr_if(self);
    } else if (parse_match(self, TOK_WHILE)) {
        return expr_while(self);
    }

    return int_expr_bp(self);
}

AstIndex int_expr_bp(self_t) {
    ParseFrame top = {
        .min_bp = 0,
        .lhs = expr_literal(self),
        .op_idx = UINT32_MAX,
    };

    ParseFrameStack stack;
    // Freed in the single return below. Must be careful about returns here.
    parse_frame_stack_init(&stack);

    for (;;) {
        Token token = parse_peek_curr(self);
        BindingPower bp = token_bp(token, top.lhs == ast_index_empty);

        bool is_not_op = bp.lhs == 0; // We return 0, 0 if it was not a valid operator, and none of them return 0 for LHS.
        bool is_low_bp = bp.lhs < top.min_bp;// Too low of binding power (precedence) to continue.
        if (is_not_op || is_low_bp) {
            ParseFrame res = top;
            if (parse_frame_stack_empty(&stack)) {
                parse_frame_stack_free(&stack);
                if (res.lhs == ast_index_empty) {
                    return error(self, AST_ERR_UNEXPECTED_EOF);
                }
                return res.lhs;
            }

            top = parse_frame_stack_pop(&stack);

            // This is kind of a hack, we need to treat lparen as a call, not parens when its not in a prefix position.
            bool is_postfix = res.min_bp == 100;
            if (self->tokens.data[res.op_idx].type == TOK_LPAREN && !is_postfix) {
                assert(parse_advance(self).type == TOK_RPAREN);
                top.lhs = res.lhs;
                continue;
            }

            // Account for special case of prefix operators.
            // AST unary operator is normalized to only have a data.lhs, and no RHS.
            // but prefix operators are the opposite, only RHS. So flip that.
            AstIndex lhs = top.lhs, rhs = res.lhs;
            if (lhs == ast_index_empty) {
                lhs = rhs;
                rhs = ast_index_empty;
            }

            // Determine the appropriate node type, default to binary if there is no special case.
            AstTag tag = AST_BINARY;
            Token op_token = self->tokens.data[res.op_idx];
            if (op_token.type == TOK_DOT) {
                tag = AST_DOT;
            } else if (op_token.type == TOK_LPAREN) {
                // If it is a paren, the lparen does not make it here. It is flattened above.
                tag = AST_CALL;
            } else if (rhs == ast_index_empty) {
                // Unary operators only ever have a LHS, this is normalized between prefix/postfix above.
                tag = AST_UNARY;
            }

            // Write the node to the node array.
            ast_node_list_add(&self->nodes, (AstNode) {
                .tag = tag,
                .main_token = res.op_idx,
                .data = {lhs, rhs},
            });
            top.lhs = self->nodes.size - 1;
            continue;
        }

        // This rhs param is somewhat confusing and I am not in love. It is used to specify a
        //  rhs for a postfix operator. In particular, AST_CALL has AstCallData in its rhs.
        // In practice, what it does is override the creation of the RHS. For a postfix operator,
        //  the following expression will always be empty, so we just fill it with call data.
        AstIndex rhs = ast_index_empty;
        TokenIndex op_idx = self->tok_index;
        if (self->tokens.data[op_idx].type == TOK_LPAREN && top.lhs != UINT32_MAX) {
            rhs = call_data(self);// See above comment
        } else {
            parse_advance(self);// Eat the operator symbol
        }

        // If an override (eg call data) was not specified, try to parse a literal following the operator.
        //  This fills the default case of 1 + 2 for example.
        if (rhs == ast_index_empty)
            rhs = expr_literal(self);
        parse_frame_stack_push(&stack, top);
        top = (ParseFrame) {
            .min_bp = bp.rhs,
            .lhs = rhs,
            .op_idx = op_idx,
        };
    }
}

AstIndex expr_literal(self_t) {
    Token next = parse_peek_curr(self);

    // If we are at EOF, nothing can be parsed
    if (next.type == TOK_EOF) {
        return parse_error(self);
    }


    if (next.type == TOK_NUMBER) {
        parse_advance(self);
        ast_node_list_add(&self->nodes, (AstNode) {
            .tag = AST_INTEGER,
            .main_token = self->tok_index - 1,
            .data = {ast_index_empty, ast_index_empty},
        });
        return self->nodes.size - 1;
    } else if (next.type == TOK_STRING) {
        parse_advance(self);
        ast_node_list_add(&self->nodes, (AstNode) {
            .tag = AST_STRING,
            .main_token = self->tok_index - 1,
            .data = {ast_index_empty, ast_index_empty},
        });
        return self->nodes.size - 1;
    } else if (next.type == TOK_TRUE || next.type == TOK_FALSE) {
        parse_advance(self);
        ast_node_list_add(&self->nodes, (AstNode) {
            .tag = AST_BOOL,
            .main_token = self->tok_index - 1,
            .data = {ast_index_empty, ast_index_empty},
        });
        return self->nodes.size - 1;
    } else if (next.type == TOK_IDENT) {
        parse_advance(self);
        ast_node_list_add(&self->nodes, (AstNode) {
            .tag = AST_REF,
            .main_token = self->tok_index - 1,
            .data = {ast_index_empty, ast_index_empty},
        });
        return self->nodes.size - 1;
    }

    return ast_index_empty;
}

AstIndex expr_block(self_t) {
    assert(parse_peek_curr(self).type == TOK_LBRACE);
    TokenIndex main_token = self->tok_index;

    AstIndexPair data = int_parse_list(self, int_stmt,
                                       TOK_LBRACE, TOK_RBRACE, TOK_SEMI);

    // Create the block node
    ast_node_list_add(&self->nodes, (AstNode) {
        .tag = AST_BLOCK,
        .main_token = main_token,
        .data = {data.first, data.second},
    });
    return self->nodes.size - 1;
}

AstIndex expr_return(self_t) {
    TokenIndex main_token = parse_assert(self, TOK_RETURN);

    AstIndex expr = ast_index_empty;
    TokenType next = parse_peek_curr(self).type;
    // Semicolon to manually terminate, rbrace if it is last in a block.
    if (next != TOK_SEMI && next != TOK_RBRACE && next != TOK_EOF) {
        expr = int_expr(self);
    }

    ast_node_list_add(&self->nodes, (AstNode) {
        .tag = AST_RETURN,
        .main_token = main_token,
        .data = {expr, ast_index_empty},
    });
    return self->nodes.size - 1;
}

AstIndex expr_if(self_t) {
    TokenIndex main_token = parse_assert(self, TOK_IF);

    AstIndex cond = int_expr(self);

    AstIndex then = expr_block(self);

    AstIndex else_ = ast_index_empty;
    if (parse_match_advance(self, TOK_ELSE)) {
        else_ = parse_match(self, TOK_IF) ? expr_if(self) : expr_block(self);
    }

    AstIfData data = {then, else_};
    AstIndex data_idx = self->extra_data.size;
    index_list_add_sized(&self->extra_data, data);

    ast_node_list_add(&self->nodes, (AstNode) {
        .tag = AST_IF,
        .main_token = main_token,
        .data = {cond, data_idx},
    });
    return self->nodes.size - 1;
}

AstIndex expr_while(self_t) {
    TokenIndex main_token = parse_assert(self, TOK_WHILE);

    AstIndex cond = int_expr(self);

    AstIndex body = expr_block(self);

    ast_node_list_add(&self->nodes, (AstNode) {
        .tag = AST_WHILE,
        .main_token = main_token,
        .data = {cond, body},
    });
    return self->nodes.size - 1;
}

//endregion

//region Type expressions

AstIndex type_expr_constant(self_t) {
    // Check for ptr type
    if (parse_match(self, TOK_STAR)) {
        TokenIndex main_token = parse_assert(self, TOK_STAR);

        AstIndex inner_type = type_expr_constant(self);

        ast_node_list_add(&self->nodes, (AstNode) {
            .tag = AST_TYPE,
            .main_token = main_token,
            .data = {inner_type, ast_index_empty},
        });
        return self->nodes.size - 1;
    } else {
        TokenIndex main_token = parse_assert(self, TOK_IDENT);

        ast_node_list_add(&self->nodes, (AstNode) {
            .tag = AST_TYPE,
            .main_token = main_token,
            .data = {ast_index_empty, ast_index_empty},
        });
        return self->nodes.size - 1;
    }
}

//endregion

//region Special

AstIndex fn_proto(self_t, bool foreign) {
    TokenIndex main_token = parse_assert(self, TOK_IDENT);

    // Parse parameters
    AstIndexPair param_data = int_parse_list(self, fn_param,
                                             TOK_LPAREN, TOK_RPAREN, TOK_COMMA);

    // Parse type expression
    AstIndex type_expr = ast_index_empty;
    if (parse_peek_curr(self).type != TOK_LBRACE) {
        type_expr = type_expr_constant(self);
    }

    uint32_t flags = FN_PROTO_NONE;
    if (foreign) flags |= FN_PROTO_FOREIGN;

    AstIndex proto_data_idx = self->extra_data.size;
    AstFnProto proto_data = {
        .flags = flags,
        .param_start = param_data.first,
        .param_end = param_data.second,
    };
    index_list_add_sized(&self->extra_data, proto_data);

    ast_node_list_add(&self->nodes, (AstNode) {
        .tag = AST_FN_PROTO,
        .main_token = main_token,
        .data = {proto_data_idx, type_expr},
    });
    return self->nodes.size - 1;
}

AstIndex fn_param(self_t) {
    TokenIndex main_token = parse_assert(self, TOK_IDENT);

    // Parse type expression
    AstIndex type_expr = ast_index_empty;
    if (parse_match_advance(self, TOK_COLON)) {
        type_expr = type_expr_constant(self);
    }

    ast_node_list_add(&self->nodes, (AstNode) {
        .tag = AST_FN_PARAM,
        .main_token = main_token,
        .data = {ast_index_empty, type_expr},
    });
    return self->nodes.size - 1;
}

AstIndex struct_field(self_t) {
    TokenIndex main_token = parse_assert(self, TOK_IDENT);

    // Parse type expression
    AstIndex type_expr = ast_index_empty;
    //todo

    ast_node_list_add(&self->nodes, (AstNode) {
        .tag = AST_FIELD,
        .main_token = main_token,
        .data = {ast_index_empty, type_expr},
    });
    return self->nodes.size - 1;
}

AstIndex enum_case(self_t) {
    TokenIndex main_token = parse_assert(self, TOK_IDENT);

    ast_node_list_add(&self->nodes, (AstNode) {
        .tag = AST_ENUM_CASE,
        .main_token = main_token,
        .data = {ast_index_empty, ast_index_empty},
    });
    return self->nodes.size - 1;
}

AstIndex call_data(self_t) {
    // Parse parameters
    AstIndexPair param_data = int_parse_list(self, int_expr,
                                             TOK_LPAREN, TOK_RPAREN, TOK_COMMA);

    AstIndex call_data_idx = self->extra_data.size;
    AstCallData call_data = {
        .arg_start = param_data.first,
        .arg_end = param_data.second,
    };
    index_list_add_sized(&self->extra_data, call_data);

    return call_data_idx;
}

//endregion

// Unmapped

AstIndexPair
int_parse_list(self_t, AstIndex (*parse_fn)(self_t), TokenType open, TokenType close, TokenType delimiter) {
    parse_assert(self, open);

    // Need to store the inner indices so that they can all be added at once to ensure
    //  they are continuous inside extra_data. Consider the case of {{x}}.
    IndexList inner_indices;
    index_list_init(&inner_indices);

    // Parse inner expressions
    while (parse_peek_curr(self).type != close) {
        AstIndex idx = parse_fn(self);
        index_list_add(&inner_indices, idx);

        if (idx == ast_index_empty) {
            //todo probably need to add an error? Although this may be unreachable
            assert(false);
        }

        if (parse_peek_curr(self).type == delimiter) {
            parse_advance(self);
        } else if (parse_peek_curr(self).type == close) {
            break;
        } else {
            // Insert an error, however we can still continue trying to parse this
            //  eg this is a non-fatal parse error.
            error_list_add(&self->errors, (CompileError) {
                .error_code = AST_ERR_MISSING_SEMICOLON, //todo this isnt always a semicolon, depends on `delimiter`
                .node = ast_index_empty,
                .location = {parse_peek_curr(self).loc.start - (size_t) self->source, UINT32_MAX},
                .data = NULL,
            });
        }
    }
    parse_assert(self, close);

    // First node
    AstIndex start = ast_index_empty;
    AstIndex end = ast_index_empty;

    if (inner_indices.size != 0) {
        start = self->extra_data.size;
        end = self->extra_data.size + inner_indices.size - 1;

        // Copy inner_indices to extra_data
        for (size_t i = 0; i < inner_indices.size; i++) {
            index_list_add(&self->extra_data, inner_indices.data[i]);
        }
    }

    index_list_free(&inner_indices);
    return (AstIndexPair) {start, end};
}


// Pratt BP

BindingPower token_bp(Token token, bool is_prefix) {
    switch (token.type) {
        case TOK_AMPAMP:
        case TOK_BARBAR:
            return (BindingPower) {3, 4};
        case TOK_EQEQ:
        case TOK_BANGEQ:
        case TOK_GT:
        case TOK_GTEQ:
        case TOK_LT:
        case TOK_LTEQ:
            return (BindingPower) {5, 6};
        case TOK_PLUS:
        case TOK_MINUS:
            return !is_prefix ? (BindingPower) {15, 16} : (BindingPower) {99, 19};
        case TOK_STAR:
        case TOK_SLASH:
            return (BindingPower) {17, 18};
        case TOK_BANG://todo unused postfix, remove me.
            return (BindingPower) {21, 100};
        case TOK_DOT:
            return (BindingPower) {41, 42};
        case TOK_LPAREN:
            return is_prefix ? (BindingPower) {99, 0} : (BindingPower) {19, 100};
        default:
            return (BindingPower) {0, 0};
    }
}


#undef self_t


// Parse frame/stack

#define self_t ParseFrameStack *self

void parse_frame_stack_init(self_t) {
    self->size = 0;
    self->capacity = 0;
    self->data = NULL;
}

void parse_frame_stack_free(self_t) {
    ARRAY_FREE(ParseFrame, self->data);
    parse_frame_stack_init(self);
}

void parse_frame_stack_push(self_t, ParseFrame frame) {
    if (self->size == self->capacity) {
        self->capacity = ARRAY_GROW_CAPCITY(self->capacity);
        self->data = ARRAY_GROW(ParseFrame, self->data, self->capacity);
    }
    self->data[self->size] = frame;
    self->size++;
}

ParseFrame parse_frame_stack_pop(self_t) {
    assert(self->size > 0);
    self->size--;
    return self->data[self->size];
}

bool parse_frame_stack_empty(self_t) {
    return self->size == 0;
}

#undef self_t
