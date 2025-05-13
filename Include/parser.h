#pragma once
#include <expression.h>
#include <statement.h>
#include <ast.h>

typedef struct Parser {
    SPL_Token* tokens;
    SPL_Token tok;
    int current;
} Parser;

ASTNode* parse(Parser* parser) {
    ASTNode* ast = parser_expression_parse(parser);
}