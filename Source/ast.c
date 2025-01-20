#pragma once
#include <ast.h>

AST astPrimaryIntegerCreate(int num) {
    AST ast;
    ast.type = AST_PRIMARY;
    ast.primary = primaryIntegerCreate(num);

    return ast;
}

AST astPrimaryFloatCreate(float num) {
    AST ast;
    ast.type = AST_PRIMARY;
    ast.primary = primaryFloatCreate(num);
    
    return ast;
}

AST astUnaryCreate(char op, AST* operand) {
    AST ast;
    ast.type = AST_URARY;
    ast.unary = unaryCreate(op, operand);

    return ast;
}

AST astTermCreate(char op, AST* left, AST* right) {
    AST ast;
    ast.type = AST_TERM;
    ast.term = termCreate(op, left, right);
}

AST astFactorCreate(char op, AST* left, AST* right) {
    AST ast;
    ast.type = AST_FACTOR;
    ast.factor = factorCreate(op, left, right);
}

AST astGroupingCreate(AST* expression) {
    AST ast;
    ast.type = AST_GROUPING;
    ast.group = groupCreate(op, left, right);
}