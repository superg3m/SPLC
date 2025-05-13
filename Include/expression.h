#pragma once
#include <token.h>

typedef struct Expression Expression;

typedef struct StringExpression {
    CKG_StringView name;
    int line;
} StringExpression;

typedef struct IntegerExpression {
    int value;
    int line;
} IntegerExpression;

typedef struct FloatExpression {
    double value;
    int line;
} FloatExpression;

typedef struct BoolExpression {
    bool value;
    int line;
} BoolExpression;

typedef struct IdentifierExpression {
    CKG_StringView name;
    int line;
} IdentifierExpression;

typedef struct UnaryOperationExpression {
    SPL_Token operation;
    Expression* operand;
    int line;
} UnaryOperationExpression;

typedef struct BinaryOperationExpression {
    SPL_Token operation;
    Expression* left;
    Expression* right;
    int line;
} BinaryOperationExpression;

typedef struct LogicalOperationExpression {
    SPL_Token operation;
    Expression* left;
    Expression* right;
    int line;
} LogicalOperationExpression;

typedef struct GroupingExpression {
    Expression* value;
    int line;
} GroupingExpression;

typedef enum ExpressionType {
    EXPRESSION_TYPE_STRING,
    EXPRESSION_TYPE_INTEGER,
    EXPRESSION_TYPE_FLOAT,
    EXPRESSION_TYPE_BOOLEAN,
    EXPRESSION_TYPE_IDENTIFER,
    EXPRESSION_TYPE_UNARY_OPERATION,
    EXPRESSION_TYPE_BINARY_OPERATION,
    EXPRESSION_TYPE_LOGICAL_OPERATION,
    EXPRESSION_TYPE_GROUPING
} ExpressionType;

typedef struct Expression {
    ExpressionType type;
    union {
        StringExpression* str;
        IntegerExpression* integer;
        FloatExpression* floating;
        BoolExpression* boolean;
        IdentifierExpression* identifier;
        UnaryOperationExpression* unary;
        BinaryOperationExpression* binary;
        LogicalOperationExpression* logical;
        GroupingExpression* grouping;
    };
} Expression;

Expression* string_expression_create(CKG_StringView name, int line);
Expression* integer_expression_create(int value, int line);
Expression* float_expression_create(double value, int line);
Expression* bool_expression_create(bool value, int line);
Expression* identifier_expression_create(CKG_StringView name, int line);
Expression* unary_expression_create(SPL_Token operation, Expression* operand, int line);
Expression* binary_expression_create(SPL_Token operation, Expression* left, Expression* right, int line);
Expression* logical_expression_create(SPL_Token operation, Expression* left, Expression* right, int line);
Expression* grouping_expression_create(Expression* value, int line);