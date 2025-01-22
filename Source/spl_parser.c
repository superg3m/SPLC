#include <spl_parser.h>

Expression* expressionPrimaryIntegerCreate(CKIT_Arena* parser_arena, int num);
Expression* expressionPrimaryFloatCreate(CKIT_Arena* parser_arena, float num);
Expression* expressionUnaryCreate(CKIT_Arena* parser_arena, SPL_Token op, Expression* operand);
Expression* expressionTermCreate(CKIT_Arena* parser_arena, SPL_Token op, Expression* left, Expression* right);
Expression* expressionFactorCreate(CKIT_Arena* parser_arena, SPL_Token op, Expression* left, Expression* right);
Expression* expressionGroupingCreate(CKIT_Arena* parser_arena, Expression* expression);

Expression* parseExpression(Parser* parser);

Parser parserCreate() {
    Parser ret;
    ret.tokens = NULLPTR;
    ret.current = 0;
    ret.tok;
    ret.arena_allocator = ckit_arena_create(KiloBytes(2), "Parser Allocator");

    return ret;
}

void parserFree(Parser* parser) {
    ckit_arena_free(parser->arena_allocator);
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

// <Primary>    ::= <integer> | <float> | '(' <Expression> ')'
internal Expression* parsePrimary(Parser* parser) {
    if (consumeOnMatch(parser, SPL_TOKEN_INTEGER_LITERAL)) { 
        int num = atoi(previousToken(parser).lexeme);
        return expressionPrimaryIntegerCreate(parser->arena_allocator, num);
    } else if (consumeOnMatch(parser, SPL_TOKEN_FLOAT_LITERAL)) {
        float num = (float)atof(previousToken(parser).lexeme);
        return expressionPrimaryFloatCreate(parser->arena_allocator, num);
    } else if (consumeOnMatch(parser, SPL_TOKEN_LEFT_PAREN)) {
        Expression* expression = parseExpression(parser);
        if (!consumeOnMatch(parser, SPL_TOKEN_RIGHT_PAREN)) {
            reportError(parser, "Error: ')' expected");
        } else  {
            return expressionGroupingCreate(parser->arena_allocator, expression);
        }
    }

    return NULLPTR;
}


// <Unary>      ::= ('+'|'-'|'!') <Primary> | <Primary>
Expression* parseUnary(Parser* parser) {
    if (consumeOnMatch(parser, SPL_TOKEN_NOT) || consumeOnMatch(parser, SPL_TOKEN_MINUS) || consumeOnMatch(parser, SPL_TOKEN_PLUS)) {
        SPL_Token op = previousToken(parser);
        Expression* operand = parsePrimary(parser);
        return expressionUnaryCreate(parser->arena_allocator, op, operand);
    }

    return parsePrimary(parser);
}

internal Expression* binaryOperation(Parser* parser, SPL_Token op, Expression* left, Expression* right) {
    char operation = op.lexeme[0];
    switch (operation) {
        case '+':
        case '-': {
            return expressionTermCreate(parser->arena_allocator, op, left, right);
        } break;

        case '*':
        case '/': {
            return expressionFactorCreate(parser->arena_allocator, op, left, right);
        } break;
    }

    ckit_assert(FALSE); // This shouldn't happen?
    return NULLPTR;
}

// <Factor>     ::= <Unary> (('*'|'/') <Unary>)*
internal Expression* parseFactor(Parser* parser) {
    Expression* expr = parseUnary(parser);

    while (consumeOnMatch(parser, SPL_TOKEN_STAR) || consumeOnMatch(parser, SPL_TOKEN_DIVISION)) {
        SPL_Token op = previousToken(parser);
        Expression* right = parseUnary(parser);
        expr = binaryOperation(parser, op, expr, right);
    }

    return expr;
}

// <Term>       ::= <Factor> (('+'|'-') <Factor>)*
internal Expression* parseTerm(Parser* parser) {
    Expression* expr = parseFactor(parser);

    while (consumeOnMatch(parser, SPL_TOKEN_PLUS) || consumeOnMatch(parser, SPL_TOKEN_MINUS)) {
        SPL_Token op = previousToken(parser);
        Expression* right = parseFactor(parser);
        expr = binaryOperation(parser, op, expr, right);
    }

    return expr;
}

// <Expression> ::= <Term>
internal Expression* parseExpression(Parser* parser) {
    return parseTerm(parser);
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