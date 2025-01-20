#pragma once
#include <spl_cfg.h>

typedef enum ExpressionType {
    EXPRESSION_PRIMARY,      // e.g., integer or float
    EXPRESSION_URARY,        // e.g., unary '+5' or '-4'
    EXPRESSION_TERM,         // e.g., 6 + 2
    EXPRESSION_FACTOR,       // e.g., 1 * 6
    EXPRESSION_GROUPING,     // ( <Expression> )

} ExpressionType;

typedef struct Expression {
    ExpressionType type;
    union {
        Primary primary;
        Unary unary;
        Term term;
        Factor factor;
        Grouping grouping;
    };
} Expression;

Expression expressionPrimaryIntegerCreate(int num);
Expression expressionPrimaryFloatCreate(float num);
Expression expressionUnaryCreate(char op, Expression* operand);
Expression expressionTermCreate(char op, Expression* left, Expression* right);
Expression expressionFactorCreate(char op, Expression* left, Expression* right);

// <Expression> ::= <Term>
// <Term>       ::= <Factor> (('+'|'-') <Factor>)*
// <Factor>     ::= <Unary> (('*'|'/') <Unary>)*
// <Unary>      ::= ('+'|'-') <Primary> | <Primary>
// <Primary>    ::= <integer> | <float> | '(' <Expression> ')'