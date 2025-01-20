#pragma once
#include <expression.h>

Expression expressionPrimaryIntegerCreate(int num) {
    Expression ret;
    ret.type = EXPRESSION_PRIMARY;
    ret.primary = primaryIntegerCreate(num);

    return ret;
}

Expression expressionPrimaryFloatCreate(float num) {
    Expression ret;
    ret.type = EXPRESSION_PRIMARY;
    ret.primary = primaryFloatCreate(num);
    
    return ret;
}

Expression expressionUnaryCreate(char op, Expression* operand) {
    Expression ret;
    ret.type = EXPRESSION_URARY;
    ret.unary = unaryCreate(op, operand);

    return ret;
}

Expression expressionTermCreate(char op, Expression* left, Expression* right) {
    Expression ret;
    ret.type = EXPRESSION_TERM;
    ret.term = termCreate(op, left, right);

    return ret;
}

Expression expressionFactorCreate(char op, Expression* left, Expression* right) {
    Expression ret;
    ret.type = EXPRESSION_FACTOR;
    ret.factor = factorCreate(op, left, right);

    return ret;
}

Expression expressionGroupingCreate(Expression* expression) {
    Expression ret;
    ret.type = EXPRESSION_GROUPING;
    ret.grouping = groupingCreate(expression);

    return ret;
}