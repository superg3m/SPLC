#pragma once
#include <token.h>
#include <ast.h>

typedef struct Parser {
    SPL_Token* tokens;
    int current;
    SPL_Token tok;
} Parser;

Parser createParser() {
    Parser ret;
    ret.tokens = NULLPTR;
    ret.current = 0;
    ret.tok;
}

void consumeNextToken(Parser* parser) {
    parser->tok = parser->tokens[parser->current];
    parser->current += 1;
}

SPL_Token peekNthToken(Parser* parser, int n) {
    return parser->tokens[parser->current + n];
}

internal void reportError(Parser* parser, char* msg) {
    LOG_ERROR("Parser Error: %s | Line: %d\n", peekNthToken(parser, 0).lexeme, peekNthToken(parser, 0).line);
    LOG_ERROR("Msg: %s\n", msg);
    ckit_assert(FALSE);
}

void expect(Parser* parser, SPL_TokenType expected_type) {
    if (peekNthToken(parser, 0).type != expected_type) {
        LOG_ERROR("Expected: %s | Got: %s", tokenTypeToString(expected_type), peekNthToken(parser, 0).lexeme);
        reportError(parser, "\n");
    }
}

Boolean consumeOnMatch(Parser* parser, SPL_TokenType expected_type) {
    if (peekNthToken(parser, 0).type == expected_type) {
        consumeNextToken(parser);
        return TRUE;
    }

    return FALSE;
}

SPL_Token previousToken(Parser* parser) {
    return parser->tokens[parser->current - 1];
}

// <primary>  ::=  <integer> | <float> | '(' <expr> ')'
AST primary(Parser* parser) {
    if (consumeOnMatch(parser, SPL_TOKEN_INTEGER_LITERAL)) { 
        int num = atoi(previousToken(parser).lexeme);
        return astPrimaryIntegerCreate(num);
    } else if (consumeOnMatch(parser, SPL_TOKEN_FLOAT_LITERAL)) {
        float num = atof(previousToken(parser).lexeme);
        return astPrimaryFloatCreate(num);
    } else if (consumeOnMatch(parser, SPL_TOKEN_LEFT_PAREN)) {
        AST expression = expression();
        if (!consumeOnMatch(parser, SPL_TOKEN_RIGHT_PAREN)) {
            reportError(parser, "Error: ')' expected");
        } else  {
            return astGroupingCreate(expression);
        }
    }

}

Expression binaryOperation(char op, Expression* left, Expression* right) {
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


AST parseExpression(Parser* parser) {
    Expression expr = parseTerm();

    while (consumeOnMatch(parser, SPL_TOKEN_PLUS) || consumeOnMatch(parser, SPL_TOKEN_MINUS)) {
        Expression op = previousToken(parser);
        Expression right = parseTerm(parser);

        expr = binaryOperation(expr, op, right);
    }

    return expr;
}

AST* generateAST(Parser* parser, SPL_Token* tokens) {
    parser->current = 0;
    parser->tokens = tokens;
    parser->tok = parser->tokens[parser->current];


    AST* ast = NULLPTR;
    while (parser->current < (ckit_vector_count(parser->tokens) - 1)) {
        ckit_vector_push(ast, parseExpression(parser))
    }

    return ast
}