#pragma once
#include <token.h>

typedef struct StringExpression {
    CKG_StringView name;
    int line;
} StringExpression;

typedef struct IntegerExpression {
    int value;
    int line;
} IntegerExpression;

typedef struct FloatExpression {
    float value;
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
    STRING,
    INTEGER,
    FLOAT,
    BOOL,
    IDENTIFER,
    UNARY_OPERATION,
    BINARY_OPERATION,
    LOGICAL_OPERATION,
    GROUPING,
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

StringExpression* string_expression_create(CKG_StringView name, int line);
IntegerExpression* integer_expression_create(int value, int line);
FloatExpression* float_expression_create(float value, int line);
BoolExpression* bool_expression_create(bool value, int line);
IdentifierExpression* identifier_expression_create(CKG_StringView name, int line);
UnaryOperationExpression* unary_expression_create(SPL_Token operation, Expression* operand, int line);
BinaryOperationExpression* binary_expression_create(SPL_Token operation, Expression* left, Expression* right, int line);
LogicalOperationExpression* logical_expression_create(SPL_Token operation, Expression* left, Expression* right, int line);
GroupingExpression* grouping_expression_create(Expression* value, int line);