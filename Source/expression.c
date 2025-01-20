#pragma once
#include <expression.h>

Expression* expressionPrimaryIntegerCreate(int num) {
    Expression* ret = ckit_alloc(sizeof(Expression));
    ret->type = EXPRESSION_PRIMARY;
    ret->primary.PrimaryType = PRIMARY_INTEGER;
    ret->primary.integer_num = num;

    return ret;
}

Expression* expressionPrimaryFloatCreate(float num) {
    Expression* ret = ckit_alloc(sizeof(Expression));
    ret->type = EXPRESSION_PRIMARY;
    ret->primary.PrimaryType = PRIMARY_FLOAT;
    ret->primary.float_num = num;

    return ret;
}

Expression* expressionUnaryCreate(char op, Expression* operand) {
    Expression* ret = ckit_alloc(sizeof(Expression));
    ret->type = EXPRESSION_URARY;
    ret->unary.op = op;
    ret->unary.operand = operand;

    return ret;
}

Expression* expressionTermCreate(char op, Expression* left, Expression* right) {
    Expression* ret = ckit_alloc(sizeof(Expression));
    ret->type = EXPRESSION_TERM;
    ret->term.op = op;
    ret->term.left = left;
    ret->term.right = right;

    return ret;
}

Expression* expressionFactorCreate(char op, Expression* left, Expression* right) {
    Expression* ret = ckit_alloc(sizeof(Expression));
    ret->type = EXPRESSION_FACTOR;
    ret->term.op = op;
    ret->term.left = left;
    ret->term.right = right;

    return ret;
}