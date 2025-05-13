#pragma once
#include <expression.h>
#include <statement.h>

typedef enum AST_NodeType {
    AST_NODE_EXPRESSION,
    AST_NODE_STATEMENT,
    AST_NODE_PROGRAM
} AST_NodeType;

typedef struct Program Program;

typedef struct ASTNode {
    AST_NodeType type;
    union {
        Program* program;
        Expression* expression;
        Statement* statement;
    };
} ASTNode;

ASTNode* ast_node_create(AST_NodeType type, void* node);
void ast_pretty_print(ASTNode* ast);