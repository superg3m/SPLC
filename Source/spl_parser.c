#include <spl_parser.h>

SPL_Token parser_peek_nth_token(Parser* parser, int n) {
    if (parser->current + n > ckg_vector_count(parser->tokens) - 1) {
        return SPL_TOKEN_CREATE(SPL_TOKEN_ILLEGAL_TOKEN, "", -1);
    }

    return parser->tokens[parser->current + n];
}

SPL_Token parser_consume_next_token(Parser* parser, SPL_TokenType* expected_type) {
    if (expected_type && parser_peek_nth_token(parser, 0).type != *expected_type) {
        parser_report_error(parser, f"Expected: {expected_type} | Got: {self.peekNthToken()}");
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

internal Expression* parse_comparison_expression(Parser* parser) {
    Expression* expression = parse_binary_expression();
    while (self.consumeOnMatch(TokenType.EQUALS) or
            self.consumeOnMatch(TokenType.NOT_EQUALS) or
            self.consumeOnMatch(TokenType.LESS_THAN) or
            self.consumeOnMatch(TokenType.LESS_THAN_EQUALS) or
            self.consumeOnMatch(TokenType.GREATER_THAN) or
            self.consumeOnMatch(TokenType.GREATER_THAN_EQUALS)):
        op = self.previousToken()
        right = BinOp_additive()
        expr = BinaryOperation(expr, op, right, self.previousToken().line)
}

// <logical>    ::= <comparison> (("||" | "&&") <comparison>)*
internal Expression* parse_logical_expression(Parser* parser) {
    Expression* expression = parse_comparison_expression(parser);
    while (parser->tok.type == SPL_TOKEN_OR || parser->tok.type == SPL_TOKEN_AND) {
        expression = parse_comparison_expression(parser);
        SPL_Token op = parser_previous_token(parser);
        Expression* right = parse_comparison_expression();
        expression = ast_node_create(AST_NODE_EXPRESSION, logical_expression_create(op, expression, right, op.line));
    }

    return expression;
}

// <expression> ::= <logical_expr>
internal ASTNode* parse_expression(Parser* parser) {
    return parse_logical_expression(parser);
}

ASTNode* parse(Parser* parser) {
    ASTNode* ast = parse_expression(parser);

    return ast;
}