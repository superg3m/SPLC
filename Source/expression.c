#include <expression.h>

Expression* string_expression_create(CKG_StringView name, int line) {
    Expression* ret = ckg_alloc(sizeof(Expression));
    ret->type = EXPRESSION_TYPE_STRING;
    ret->str = ckg_alloc(sizeof(StringExpression));
    ret->str->name = name;
    ret->str->line = line;
    return ret;
}

Expression* integer_expression_create(int value, int line) {
    Expression* ret = ckg_alloc(sizeof(Expression));
    ret->type = EXPRESSION_TYPE_INTEGER;
    ret->integer = ckg_alloc(sizeof(IntegerExpression));
    ret->integer->value = value;
    ret->integer->line = line;
    return ret;
}

Expression* float_expression_create(double value, int line) {
    Expression* ret = ckg_alloc(sizeof(Expression));
    ret->type = EXPRESSION_TYPE_FLOAT;
    ret->floating = ckg_alloc(sizeof(FloatExpression));
    ret->floating->value = value;
    ret->floating->line = line;
    return ret;
}

Expression* bool_expression_create(bool value, int line) {
    Expression* ret = ckg_alloc(sizeof(Expression));
    ret->type = EXPRESSION_TYPE_BOOLEAN;
    ret->boolean = ckg_alloc(sizeof(BoolExpression));
    ret->boolean->value = value;
    ret->boolean->line = line;
    return ret;
}

Expression* identifier_expression_create(CKG_StringView name, int line) {
    Expression* ret = ckg_alloc(sizeof(Expression));
    ret->type = EXPRESSION_TYPE_IDENTIFER;
    ret->identifier = ckg_alloc(sizeof(IdentifierExpression));
    ret->identifier->name = name;
    ret->identifier->line = line;
    return ret;
}

Expression* unary_expression_create(SPL_Token operation, Expression* operand, int line) {
    Expression* ret = ckg_alloc(sizeof(Expression));
    ret->type = EXPRESSION_TYPE_UNARY_OPERATION;
    ret->unary = ckg_alloc(sizeof(UnaryOperationExpression));
    ret->unary->operation = operation;
    ret->unary->operand = operand;
    ret->unary->line = line;
    return ret;
}

Expression* binary_expression_create(SPL_Token operation, Expression* left, Expression* right, int line) {
    Expression* ret = ckg_alloc(sizeof(Expression));
    ret->type = EXPRESSION_TYPE_BINARY_OPERATION;
    ret->binary = ckg_alloc(sizeof(BinaryOperationExpression));
    ret->binary->operation = operation;
    ret->binary->left = left;
    ret->binary->right = right;
    ret->binary->line = line;
    return ret;
}

Expression* logical_expression_create(SPL_Token operation, Expression* left, Expression* right, int line) {
    Expression* ret = ckg_alloc(sizeof(Expression));
    ret->type = EXPRESSION_TYPE_LOGICAL_OPERATION;
    ret->logical = ckg_alloc(sizeof(LogicalOperationExpression));
    ret->logical->operation = operation;
    ret->logical->left = left;
    ret->logical->right = right;
    ret->logical->line = line;
    return ret;
}

Expression* grouping_expression_create(Expression* value, int line) {
    Expression* ret = ckg_alloc(sizeof(Expression));
    ret->type = EXPRESSION_TYPE_GROUPING;
    ret->grouping = ckg_alloc(sizeof(GroupingExpression));
    ret->grouping->value = value;
    ret->grouping->line = line;
    return ret;
}