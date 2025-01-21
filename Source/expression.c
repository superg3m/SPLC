#pragma once
#include <expression.h>

Expression* expressionPrimaryIntegerCreate(CKIT_Arena* parser_arena, int num) {
    Expression* ret = ckit_arena_push(parser_arena, Expression);
    ret->type = EXPRESSION_PRIMARY;
    ret->primary.PrimaryType = PRIMARY_INTEGER;
    ret->primary.integer_num = num;

    return ret;
}

Expression* expressionPrimaryFloatCreate(CKIT_Arena* parser_arena, float num) {
    Expression* ret = ckit_arena_push(parser_arena, Expression);
    ret->type = EXPRESSION_PRIMARY;
    ret->primary.PrimaryType = PRIMARY_FLOAT;
    ret->primary.float_num = num;

    return ret;
}

Expression* expressionUnaryCreate(CKIT_Arena* parser_arena, SPL_Token op, Expression* operand) {
    Expression* ret = ckit_arena_push(parser_arena, Expression);
    ret->type = EXPRESSION_URARY;
    ret->unary.op = op;
    ret->unary.operand = operand;

    return ret;
}

Expression* expressionTermCreate(CKIT_Arena* parser_arena, SPL_Token op, Expression* left, Expression* right) {
    Expression* ret = ckit_arena_push(parser_arena, Expression);
    ret->type = EXPRESSION_TERM;
    ret->term.op = op;
    ret->term.left = left;
    ret->term.right = right;

    return ret;
}

Expression* expressionFactorCreate(CKIT_Arena* parser_arena, SPL_Token op, Expression* left, Expression* right) {
    Expression* ret = ckit_arena_push(parser_arena, Expression);
    ret->type = EXPRESSION_FACTOR;
    ret->term.op = op;
    ret->term.left = left;
    ret->term.right = right;

    return ret;
}

void expressionPrint(Expression* expression) {
    switch (expression->type) {
        case EXPRESSION_PRIMARY: {
            
        } break;
        
        case EXPRESSION_FACTOR: {
            char* operation = expression->factor.op.lexeme;
            float left = expression->factor.left->primary.float_num;
            int right = expression->factor.right->primary.integer_num;
            LOG_DEBUG("%f %s %d\n", left, operation, right);
        } break;
    }
}