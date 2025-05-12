#pragma once
#include <token.h>

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
    SPL_Token op;
    Expression* operand;
} Unary;

typedef struct Factor {
    SPL_Token op; 
    Expression* left; 
    Expression* right;
} Factor;

typedef struct Term {
    SPL_Token op; 
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

void expressionPrint(Expression* expression);

// <Expression> ::= <Term>
// <Term>       ::= <Factor> (('+'|'-') <Factor>)*
// <Factor>     ::= <Unary> (('*'|'/') <Unary>)*
// <Unary>      ::= ('+'|'-') <Primary> | <Primary>
// <Primary>    ::= <integer> | <float> | '(' <Expression> ')'