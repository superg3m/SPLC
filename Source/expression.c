#include <expression.h>

StringExpression* string_expression_create(CKG_StringView name, int line) {
    StringExpression* ret = ckg_alloc(sizeof(StringExpression));
    ret->name = name;
    ret->line = line;

    return ret;
}

IntegerExpression* integer_expression_create(int value, int line) {
    IntegerExpression* ret = ckg_alloc(sizeof(IntegerExpression));
    ret->value = value;
    ret->line = line;

    return ret;
}

FloatExpression* float_expression_create(float value, int line) {
    FloatExpression* ret = ckg_alloc(sizeof(FloatExpression));
    ret->value = value;
    ret->line = line;

    return ret;
}

BoolExpression* bool_expression_create(bool value, int line) {
    BoolExpression* ret = ckg_alloc(sizeof(BoolExpression));
    ret->value = value;
    ret->line = line;

    return ret;
}

IdentifierExpression* identifier_expression_create(CKG_StringView name, int line) {
    IdentifierExpression* ret = ckg_alloc(sizeof(IdentifierExpression));
    ret->name = name;
    ret->line = line;

    return ret;
}

UnaryOperationExpression* unary_expression_create(SPL_Token operation, Expression* operand, int line) {
    UnaryOperationExpression* ret = ckg_alloc(sizeof(UnaryOperationExpression));
    ret->operation = operation;
    ret->operand = operand;
    ret->line = line;

    return ret;
}

BinaryOperationExpression* binary_expression_create(SPL_Token operation, Expression* left, Expression* right, int line) {
    BinaryOperationExpression* ret = ckg_alloc(sizeof(BinaryOperationExpression));
    ret->operation = operation;
    ret->left = left;
    ret->right = right;
    ret->line = line;

    return ret;
}

LogicalOperationExpression* logical_expression_create(SPL_Token operation, Expression* left, Expression* right, int line) {
    LogicalOperationExpression* ret = ckg_alloc(sizeof(LogicalOperationExpression));
    ret->operation = operation;
    ret->left = left;
    ret->right = right;
    ret->line = line;

    return ret;
}

GroupingExpression* grouping_expression_create(Expression* value, int line) {
    GroupingExpression* ret = ckg_alloc(sizeof(GroupingExpression));
    ret->value = value;
    ret->line = line;

    return ret;
}
