#pragma once
#include <expression.h>
#include <statement.h>
#include <ast.h>

typedef struct Parser {
    SPL_Token* tokens;
    SPL_Token tok;
    int current;
} Parser;

/*
def parse(self):
    ast: list[Expression] = []
    while self.current < (len(self.tokens) - 1):
        ast.append(self.expression())
*/

ASTNode* parse(Parser* parser);