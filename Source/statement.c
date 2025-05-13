#include <statement.h>

Statement* print_statement_create(Expression* value, int line) {
    Statement* ret = ckg_alloc(sizeof(Statement));
    ret->type = STATEMENT_TYPE_PRINT;
    ret->print_statement = ckg_alloc(sizeof(PrintStatement));
    ret->print_statement->value = value;
    ret->print_statement->line = line;
    
    return ret;
}

Statement* assignment_statement_create(Expression* left, Expression* right, int line) {
    Statement* ret = ckg_alloc(sizeof(Statement));
    ret->type = STATEMENT_TYPE_ASSIGNMENT;
    ret->assignment_statement = ckg_alloc(sizeof(AssignmentStatement));
    ret->assignment_statement->left = left;
    ret->assignment_statement->right = right;
    ret->assignment_statement->line = line;

    return ret;
}

Statement* if_statement_create(Expression* value, Statement** if_code_block, Statement** else_code_block, int line) {
    Statement* ret = ckg_alloc(sizeof(Statement));
    ret->type = STATEMENT_TYPE_IF;
    ret->if_statement = ckg_alloc(sizeof(IfStatement));
    ret->if_statement->value = value;
    ret->if_statement->if_code_block = if_code_block;
    ret->if_statement->else_code_block = else_code_block;
    ret->if_statement->line = line;

    return ret;
}

Statement* while_statement_create(Expression* value, Statement** while_code_block, int line) {
    Statement* ret = ckg_alloc(sizeof(Statement));
    ret->type = STATEMENT_TYPE_WHILE;
    ret->while_statement = ckg_alloc(sizeof(WhileStatement));
    ret->while_statement->value = value;
    ret->while_statement->while_code_block = while_code_block;
    ret->while_statement->line = line;

    return ret;
}

Statement* for_statement_create(Statement* initalization, Expression* condition, Statement* increment, Statement** for_code_block, int line) {
    Statement* ret = ckg_alloc(sizeof(Statement));
    ret->type = STATEMENT_TYPE_FOR;
    ret->for_statement = ckg_alloc(sizeof(ForStatement));
    ret->for_statement->initalization = initalization;
    ret->for_statement->condition = condition;
    ret->for_statement->increment = increment;
    ret->for_statement->for_code_block = for_code_block;
    ret->for_statement->line = line;

    return ret;
}