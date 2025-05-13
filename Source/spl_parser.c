#include <spl_parser.h>

// Expression
//  └── Logical (||, &&)
//       └── Comparison (==, !=, <, >, etc.)
//            └── Additive (+, -, |, ^)
//                 └── Multiplicative (*, /, %, <<, >>)
//                      └── Unary (+, -, !, ~, &, *)
//                           └── Primary (literals, identifiers, etc.)

internal SPL_Token parser_peek_nth_token(Parser* parser, int n) {
    if (parser->current + n > ckg_vector_count(parser->tokens) - 1) {
        return SPL_TOKEN_CREATE_CUSTOM(SPL_TOKEN_ILLEGAL_TOKEN, "", -1);
    }

    return parser->tokens[parser->current + n];
}

internal void parser_report_error(Parser* parser, char* fmt, ...) {
    SPL_Token token = parser_peek_nth_token(parser, 0);

    va_list args;
    va_start(args, fmt);
    CKG_LOG_ERROR("String: %s\n", token_strings[token.type]);
    CKG_LOG_ERROR("Error Line: %d | %s", parser->tok.line, ckg_str_va_sprint(NULLPTR, fmt, args));
    va_end(args);

    ckg_assert(false);
}

internal SPL_Token parser_consume_next_token(Parser* parser) {
    parser->tok = parser->tokens[parser->current];
    parser->current += 1;

    return parser->tok;
}

internal void parser_expect(Parser* parser, SPL_TokenType expected_type) {
    if (expected_type && parser_peek_nth_token(parser, 0).type != expected_type) {
        parser_report_error(parser, "Expected: %s | Got: %s\n", token_strings[expected_type], token_strings[parser_peek_nth_token(parser, 0).type]);
    }

    parser_consume_next_token(parser);
}

internal bool parser_consume_on_match(Parser* parser, SPL_TokenType expected_type) {
    if (parser_peek_nth_token(parser, 0).type == expected_type) {
        parser_consume_next_token(parser);
        return true;
    }

    return false;
}

internal SPL_Token parser_previous_token(Parser* parser) {
    return parser->tokens[parser->current - 1];
}

internal Expression* parse_expression(Parser* parser);
// <primary> ::= INTEGER | FLOAT | TRUE | FALSE | STRING | PRIMITIVE_TYPE | IDENTIFIER | "(" <expression> ")"
internal Expression* parse_primary_expression(Parser* parser) {
    if (parser_consume_on_match(parser, SPL_TOKEN_INTEGER_LITERAL)) {
        SPL_Token tok = parser_previous_token(parser);
        return integer_expression_create(parser->tok.i, tok.line);
    } else if (parser_consume_on_match(parser, SPL_TOKEN_FLOAT_LITERAL)) {
        SPL_Token tok = parser_previous_token(parser);
        return float_expression_create(parser->tok.f, tok.line);
    } else if (parser_consume_on_match(parser, SPL_TOKEN_TRUE)) {
        return bool_expression_create(true, parser_previous_token(parser).line);
    } else if (parser_consume_on_match(parser, SPL_TOKEN_FALSE)) {
        return bool_expression_create(false, parser_previous_token(parser).line);
    } else if (parser_consume_on_match(parser, SPL_TOKEN_STRING_LITERAL)) {
        SPL_Token tok = parser_previous_token(parser);
        return string_expression_create(tok.name, tok.line);
    } else if (parser_consume_on_match(parser, SPL_TOKEN_LEFT_PAREN)) {
        Expression* expression = parse_expression(parser);
        parser_expect(parser, SPL_TOKEN_RIGHT_PAREN);

        return grouping_expression_create(expression, parser_previous_token(parser).line);
    } else {
        SPL_Token tok = parser_consume_next_token(parser);
        return identifier_expression_create(tok.name, tok.line);
    }
}

// sizeof removed for now
// <unary> ::= (("+" | "-" | "!" | "*" | "&" | "~") <unary>)* | <primary>
internal Expression* parse_unary_expression(Parser* parser) {
    if (parser_consume_on_match(parser, SPL_TOKEN_PLUS) ||
        parser_consume_on_match(parser, SPL_TOKEN_MINUS) ||
        parser_consume_on_match(parser, SPL_TOKEN_NOT) ||
        parser_consume_on_match(parser, SPL_TOKEN_STAR) ||
        parser_consume_on_match(parser, SPL_TOKEN_AMPERSAND) ||
        parser_consume_on_match(parser, SPL_TOKEN_BITWISE_NOT)) {
        SPL_Token op = parser_previous_token(parser);
        Expression* operand = parse_unary_expression(parser);

        return unary_expression_create(op, operand, op.line);
    }

    return parse_primary_expression(parser);
}

// <multiplicative> ::= <unary> (("*" | "/" | "%" | "<<" | ">>") <unary>)*
internal Expression* parse_multiplicative_expression(Parser* parser) {
    Expression* expression = parse_unary_expression(parser);

    while (parser_consume_on_match(parser, SPL_TOKEN_STAR) ||
           parser_consume_on_match(parser, SPL_TOKEN_DIVISION) ||
           parser_consume_on_match(parser, SPL_TOKEN_MODULUS) ||
           parser_consume_on_match(parser, SPL_TOKEN_BITWISE_LEFT_SHIFT) ||
           parser_consume_on_match(parser, SPL_TOKEN_BITWISE_RIGHT_SHIFT)) {
        SPL_Token op = parser_previous_token(parser);
        Expression* right = parse_unary_expression(parser);

        expression = binary_expression_create(op, expression, right, op.line);
    }

    return expression;
}

internal Expression* parse_additive_expression(Parser* parser) {
    Expression* expression = parse_multiplicative_expression(parser);
    while (parser_consume_on_match(parser, SPL_TOKEN_PLUS) ||
           parser_consume_on_match(parser, SPL_TOKEN_MINUS) ||
           parser_consume_on_match(parser, SPL_TOKEN_BITWISE_OR) ||
           parser_consume_on_match(parser, SPL_TOKEN_BITWISE_XOR) ||
           parser_consume_on_match(parser, SPL_TOKEN_AMPERSAND)) {

        SPL_Token op = parser_previous_token(parser);
        Expression* right = parse_multiplicative_expression(parser);

        expression = binary_expression_create(op, expression, right, op.line);
    }

    return expression;
}

// <comparison> ::= <additive> (("==" | "!=" | "<" | "<=" | ">" | ">=") <additive>)*
internal Expression* parse_comparison_expression(Parser* parser) {
    Expression* expression = parse_additive_expression(parser);
    while (parser_consume_on_match(parser, SPL_TOKEN_EQUALS) || parser_consume_on_match(parser, SPL_TOKEN_NOT_EQUALS) ||
           parser_consume_on_match(parser, SPL_TOKEN_LESS_THAN) || parser_consume_on_match(parser, SPL_TOKEN_LESS_THAN_EQUALS) ||
           parser_consume_on_match(parser, SPL_TOKEN_GREATER_THAN) || parser_consume_on_match(parser, SPL_TOKEN_GREATER_THAN_EQUALS)) {
        SPL_Token op = parser_previous_token(parser);
        Expression* right = parse_additive_expression(parser);

        expression = binary_expression_create(op, expression, right, op.line);
    }

    return expression;
}

// <logical>    ::= <comparison> (("||" | "&&") <comparison>)*
internal Expression* parse_logical_expression(Parser* parser) {
    Expression* expression = parse_comparison_expression(parser);
    while (parser_consume_on_match(parser, SPL_TOKEN_OR) || parser_consume_on_match(parser, SPL_TOKEN_AND)) {
        SPL_Token op = parser_previous_token(parser);
        Expression* right = parse_comparison_expression(parser);

        expression = logical_expression_create(op, expression, right, op.line);
    }

    return expression;
}

// <expression> ::= <logical_expr>
internal Expression* parse_expression(Parser* parser) {
    return parse_logical_expression(parser);
}

Statement* parse_statement(Parser* parser, bool requires_semi_colon);

Statement** parse_scope(Parser* parser) {
    Statement** code_block = NULLPTR;

    parser_expect(parser, SPL_TOKEN_LEFT_CURLY);
    while (parser_peek_nth_token(parser, 0).type != SPL_TOKEN_RIGHT_CURLY) {
        if (parser->current >= ckg_vector_count(parser->tokens) - 1) {
            parser_report_error(parser, "Missing closing brace\n");
        }

        ckg_vector_push(code_block, parse_statement(parser, true));
    }

    parser_expect(parser, SPL_TOKEN_RIGHT_CURLY);
    return code_block;
}


static Statement* parse_if_statement(Parser* parser) {
    parser_expect(parser, SPL_TOKEN_IF);
    parser_expect(parser, SPL_TOKEN_LEFT_PAREN);
    Expression* condition = parse_expression(parser);
    parser_expect(parser, SPL_TOKEN_RIGHT_PAREN);
    Statement** if_code_block = parse_scope(parser);

    Statement** else_code_block = NULL;
    if (parser_consume_on_match(parser, SPL_TOKEN_ELSE)) {
        else_code_block = parse_scope(parser);
    }

    return if_statement_create(condition, if_code_block, else_code_block, parser_previous_token(parser).line);
}

static Statement* parse_while_statement(Parser* parser) {
    parser_consume_on_match(parser, SPL_TOKEN_WHILE);
    parser_consume_on_match(parser, SPL_TOKEN_LEFT_PAREN);
    Expression* condition = parse_expression(parser);
    parser_consume_on_match(parser, SPL_TOKEN_RIGHT_PAREN);
    Statement** body = parse_scope(parser);

    return while_statement_create(condition, body, parser_previous_token(parser).line);
}

static Statement* parse_print_statement(Parser* parser) {
    parser_consume_on_match(parser, SPL_TOKEN_PRINT);
    parser_consume_on_match(parser, SPL_TOKEN_LEFT_PAREN);
    Expression* expr = parse_expression(parser);
    parser_consume_on_match(parser, SPL_TOKEN_RIGHT_PAREN);
    parser_consume_on_match(parser, SPL_TOKEN_SEMI_COLON);

    return print_statement_create(expr, parser_previous_token(parser).line);
}

static Statement* parse_for_statement(Parser* parser) {
    parser_consume_on_match(parser, SPL_TOKEN_FOR);
    parser_consume_on_match(parser, SPL_TOKEN_LEFT_PAREN);

    Statement* init = parse_statement(parser, true);
    Expression* condition = parse_expression(parser);
    parser_consume_on_match(parser, SPL_TOKEN_SEMI_COLON);
    Statement* increment = parse_statement(parser, false);

    parser_consume_on_match(parser, SPL_TOKEN_RIGHT_PAREN);
    Statement** body = parse_scope(parser);

    return for_statement_create(init, condition, increment, body, parser_previous_token(parser).line);
}

static Statement* parse_assignment_statement(Parser* parser, bool requires_semi_colon) {
    Expression* left = parse_expression(parser);
    parser_consume_on_match(parser, SPL_TOKEN_ASSIGNMENT);
    Expression* right = parse_expression(parser);

    if (requires_semi_colon) {
        parser_expect(parser, SPL_TOKEN_SEMI_COLON);
    }

    return assignment_statement_create(left, right, parser_previous_token(parser).line);
}

Statement* parse_statement(Parser* parser, bool requires_semi_colon) {
    SPL_TokenType next_token_type = parser_peek_nth_token(parser, 0).type;

    if (next_token_type == SPL_TOKEN_PRINT) {
        return parse_print_statement(parser);
    } else if (next_token_type == SPL_TOKEN_IF) {
        return parse_if_statement(parser);
    } else if (next_token_type == SPL_TOKEN_FOR) {
        return parse_for_statement(parser);
    } else if (next_token_type == SPL_TOKEN_WHILE) {
        return parse_while_statement(parser);
    } else if (next_token_type == SPL_TOKEN_IDENTIFIER) {
        if (parser_peek_nth_token(parser, 1).type == SPL_TOKEN_ASSIGNMENT) {
            return parse_assignment_statement(parser, requires_semi_colon);
        }
    }

    parser_report_error(parser, "No statements to interpret; this is not a valid program.\n");
    return NULL;
}

Program* program(Parser* parser) {
    Statement** statements = NULLPTR;
    while (parser->current < ckg_vector_count(parser->tokens) - 1) {
        ckg_vector_push(statements, parse_statement(parser, true));
    }

    return program_create(statements);
}

ASTNode* parse(SPL_Token* token_stream) {
    Parser parser = {0};
    parser.tokens = token_stream;
    ASTNode* ast = ast_node_create(AST_NODE_PROGRAM, (void*)program(&parser));

    return ast;
}