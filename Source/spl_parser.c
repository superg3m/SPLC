#include <spl_parser.h>

// Expression
//  └── Logical (||, &&)
//       └── Comparison (==, !=, <, >, etc.)
//            └── Additive (+, -, |, ^)
//                 └── Multiplicative (*, /, %, <<, >>)
//                      └── Unary (+, -, !, ~, &, *)
//                           └── Primary (literals, identifiers, etc.)

#define parser_report_error(parser, fmt, ...) \
CKG_LOG_ERROR("String: %s", token_strings[parser_peek_nth_token(parser, 0).type]);               \
CKG_LOG_ERROR("Error Line: %d | %s", parser_peek_nth_token(parser, 0).line, fmt, ##__VA_ARGS__); \
ckg_assert(false)                                                                                \

SPL_Token parser_peek_nth_token(Parser* parser, int n) {
    if (parser->current + n > ckg_vector_count(parser->tokens) - 1) {
        return SPL_TOKEN_CREATE_CUSTOM(SPL_TOKEN_ILLEGAL_TOKEN, "", -1);
    }

    return parser->tokens[parser->current + n];
}

SPL_Token parser_consume_next_token(Parser* parser, SPL_TokenType* expected_type) {
    if (expected_type && parser_peek_nth_token(parser, 0).type != *expected_type) {
        parser_report_error(parser, "Expected: %s | Got: %s", token_strings[*expected_type], token_strings[parser_peek_nth_token(parser, 0).type]);
    }

    parser->tok = parser->tokens[parser->current];
    parser->current += 1;

    return parser->tok;
}

bool parser_consume_on_match(Parser* parser, SPL_TokenType expected_type) {
    if (parser_peek_nth_token(parser, 0).type == expected_type) {
        parser_consume_next_token(parser, NULLPTR);
        return true;
    }


    return false;
}

SPL_Token parser_previous_token(Parser* parser) {
    return parser->tokens[parser->current - 1];
}

Expression* parse_expression(Parser* parser);
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
        if (!parser_consume_on_match(parser, SPL_TOKEN_RIGHT_PAREN)) {
            parser_report_error(parser, "Expected ')'");
        }

        return grouping_expression_create(expression, parser_previous_token(parser).line);
    } else {
        SPL_Token tok = parser_consume_next_token(parser, NULLPTR);
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

ASTNode* parse(SPL_Token* token_stream) {
    Parser parser = {0};
    parser.tokens = token_stream;
    ASTNode* ast = ast_node_create(AST_NODE_EXPRESSION, (void*)parse_expression(&parser));

    return ast;
}