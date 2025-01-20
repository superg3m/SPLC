#pragma once
#include <token.h>
#include <expression.h>

typedef struct Parser {
    SPL_Token* tokens;
    int current;
    SPL_Token tok;
} Parser;

Parser parserCreate();
Expression* generateAST(Parser* parser, SPL_Token* tokens);