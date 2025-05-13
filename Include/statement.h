#pragma once
#include <token.h>

typedef struct Statement Statement;
typedef struct Expression Expression;

typedef struct PrintStatement {
    Expression* value;
    int line;
} PrintStatement;

typedef struct AssignmentStatement {
    Expression* left;
    Expression* right;
    int line;
} AssignmentStatement;

typedef struct IfStatement {
    Expression* value;
    Statement** if_code_block;
    Statement** else_code_block;
    int line;
} IfStatement;

typedef struct WhileStatement {
    Expression* value;
    Statement** while_code_block;
    int line;
} WhileStatement;

typedef struct ForStatement {
    Statement* initalization;
    Expression* condition;
    Statement* increment;
    Statement** for_code_block;
    int line;
} ForStatement;

typedef enum StatementType {
    STATEMENT_TYPE_PRINT,
    STATEMENT_TYPE_ASSIGNMENT,
    STATEMENT_TYPE_IF,
    STATEMENT_TYPE_WHILE,
    STATEMENT_TYPE_FOR
} StatementType;

typedef struct Statement {
    StatementType type;
    union {
        PrintStatement* print_statement;
        AssignmentStatement* assignment_statement;
        IfStatement* if_statement;
        WhileStatement* while_statement;
        ForStatement* for_statement;
    };
} Statement;

Statement* print_statement_create(Expression* value, int line);
Statement* assignment_statement_create(Expression* left, Expression* right, int line);
Statement* if_statement_create(Expression* value, Statement** if_code_block, Statement** else_code_block, int line);
Statement* while_statement_create(Expression* value, Statement** while_code_block, int line);
Statement* for_statement_create(Statement* initalization, Expression* condition, Statement* increment, Statement** for_code_block, int line);