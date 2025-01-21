#pragma once
#include <token.h>
#include <expression.h>

typedef struct Parser {
    SPL_Token* tokens;
    SPL_Token tok;
    int current;
} Parser;

Parser parserCreate();
Expression* generateAST(Parser* parser, SPL_Token* tokens);