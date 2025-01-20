#pragma once
#include <ckit.h>

typedef enum ExpressionType {
    EXPRESSION_PRIMARY,      // e.g., integer or float
    EXPRESSION_URARY,        // e.g., unary '+5' or '-4'
    EXPRESSION_TERM,         // e.g., 6 + 2
    EXPRESSION_FACTOR,       // e.g., 1 * 6
    EXPRESSION_GROUPING,     // ( <Expression> )
} ExpressionType;

typedef struct Expression Expression;

typedef struct Primary {
    enum {
        PRIMARY_INTEGER, 
        PRIMARY_FLOAT
    } PrimaryType; 

    union {
        float float_num; 
        int integer_num;
    };
} Primary;

typedef struct Unary {
    char op;
    Expression* operand;
} Unary;

typedef struct Factor {
    char op; 
    Expression* left; 
    Expression* right;
} Factor;

typedef struct Term {
    char op; 
    Expression *left; 
    Expression *right;
} Term;

typedef struct Grouping {
    Expression *expression;
} Grouping;

typedef struct Expression {
    ExpressionType type;
    union {
        Primary primary;
        Unary unary;
        Factor factor;
        Term term;
        Grouping grouping;
    };
} Expression; 

// EXPRESSION_NEW(EXPRESSION_PRIMARY, Primary, primary, {PRIMARY_FLOAT, 5.6})
Expression* expressionNew(Expression expression);
Expression* expressionPrimaryIntegerCreate(int num);
Expression* expressionPrimaryFloatCreate(float num);
Expression* expressionUnaryCreate(char op, Expression* operand);
Expression* expressionTermCreate(char op, Expression* left, Expression* right);
Expression* expressionFactorCreate(char op, Expression* left, Expression* right);

// <Expression> ::= <Term>
// <Term>       ::= <Factor> (('+'|'-') <Factor>)*
// <Factor>     ::= <Unary> (('*'|'/') <Unary>)*
// <Unary>      ::= ('+'|'-') <Primary> | <Primary>
// <Primary>    ::= <integer> | <float> | '(' <Expression> ')'