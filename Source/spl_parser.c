#include <spl_parser.h>

Parser parserCreate() {
    Parser ret;
    ret.tokens = NULLPTR;
    ret.current = 0;
    ret.tok;

    return ret;
}

internal void consumeNextToken(Parser* parser) {
    parser->tok = parser->tokens[parser->current];
    parser->current += 1;
}

internal SPL_Token peekNthToken(Parser* parser, int n) {
    return parser->tokens[parser->current + n];
}

internal void reportError(Parser* parser, char* msg) {
    LOG_ERROR("Parser Error: %s | Line: %d\n", peekNthToken(parser, 0).lexeme, peekNthToken(parser, 0).line);
    LOG_ERROR("Msg: %s\n", msg);
    ckit_assert(FALSE);
}

internal void expect(Parser* parser, SPL_TokenType expected_type) {
    if (peekNthToken(parser, 0).type != expected_type) {
        LOG_ERROR("Expected: %s | Got: %s", tokenTypeToString(expected_type), peekNthToken(parser, 0).lexeme);
        reportError(parser, "\n");
    }
}

internal Boolean consumeOnMatch(Parser* parser, SPL_TokenType expected_type) {
    if (peekNthToken(parser, 0).type == expected_type) {
        consumeNextToken(parser);
        return TRUE;
    }

    return FALSE;
}

internal SPL_Token previousToken(Parser* parser) {
    return parser->tokens[parser->current - 1];
}

// <primary>  ::=  <integer> | <float> | '(' <expr> ')'
internal Expression* parsePrimary(Parser* parser) {
    if (consumeOnMatch(parser, SPL_TOKEN_INTEGER_LITERAL)) { 
        int num = atoi(previousToken(parser).lexeme);
        return expressionPrimaryIntegerCreate(num);
    } else if (consumeOnMatch(parser, SPL_TOKEN_FLOAT_LITERAL)) {
        float num = atof(previousToken(parser).lexeme);
        return expressionPrimaryFloatCreate(num);
    } else if (consumeOnMatch(parser, SPL_TOKEN_LEFT_PAREN)) {
        /*
        Expression* expression = parseExpression();
        if (!consumeOnMatch(parser, SPL_TOKEN_RIGHT_PAREN)) {
            reportError(parser, "Error: ')' expected");
        } else  {
            return astGroupingCreate(expression);
        }
        */
    }

    return NULLPTR;
}

internal Expression* parseTerm(Parser* parser) {
    /*
        expr = self.factor()
        while self.consumeOnMatch(TokenType.STAR) or self.consumeOnMatch(TokenType.DIVISION):
            op = self.previousToken()
            right = self.factor()
            expr = BinaryOperation(expr, op, right)
        return expr
    */
}

internal Expression* binaryOperation(char op, Expression* left, Expression* right) {
    switch (op) {
        case '+':
        case '-': {
            return expressionTermCreate(op, left, right);
        } break;

        case '*':
        case '/': {
            return expressionFactorCreate(op, left, right);
        } break;
    }
}

internal Expression* parseExpression(Parser* parser) {
    // Expression* expr = parseTerm();


    Expression* expr = parsePrimary(parser);

    /*
    while (consumeOnMatch(parser, SPL_TOKEN_PLUS) || consumeOnMatch(parser, SPL_TOKEN_MINUS)) {
        Expression op = previousToken(parser);
        Expression right = parseTerm(parser);

        expr = binaryOperation(expr, op, right);
    }
    */

    return expr;
}

Expression* generateAST(Parser* parser, SPL_Token* tokens) {
    parser->current = 0;
    parser->tokens = tokens;
    parser->tok = parser->tokens[parser->current];

    Expression* ast = parseExpression(parser);

    /*
    while (parser->current < (ckit_vector_count(parser->tokens) - 1)) {
        ckit_vector_push(ast, parseExpression(parser))
    }
    */

    return ast;
}