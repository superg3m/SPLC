#pragma once
#include <expression.h>

typedef enum AST_Type {
    AST_EXPRESSION,      // e.g., integer or float
    AST_STATEMENT,        // e.g., unary '+' or '-'
} AST_Type;

struct AST {
    AST_Type type;
    union {
        Expression expression;
        // Statement statement;
    };
};


// AST astExpressionCreate();
// AST astStatementCreate(float num);

