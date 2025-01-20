#pragma once

typedef struct Expression Expression;

typedef enum PrimaryType {
    PRIMARY_INTEGER,
    PRIMARY_FLOAT
} PrimaryType;

typedef struct Primary {
    PrimaryType primaryType;
    union {
        int integer_num;
        float float_num;
    };
} Primary;

Primary primaryIntegerCreate(int num) {
    Primary ret;
    ret.primaryType = PRIMARY_INTEGER;
    ret.integer_num = num;
    return ret;
}

Primary primaryFloatCreate(float num) {
    Primary ret;
    ret.primaryType = PRIMARY_FLOAT;
    ret.float_num = num;
    return ret;
}

typedef struct Unary {
    char op; // '+' | '-' | '!' | '~'
    Expression *operand;
} Unary;

Unary unaryCreate(char op, Expression* operand) {
    Unary ret;
    ret.op = op;
    ret.operand = operand;
    return ret;
}

typedef struct Binary {
    char op; // '+' | '-' | '!' | '~' | '*' | '/'
    Expression* left;
    Expression* right;
} Binary;

Binary binaryCreate(char op, Expression* left, Expression* right) {
    Binary ret;
    ret.op = op;
    ret.left = left;
    ret.right = right;
    return ret;
}

typedef struct Factor {
    char op; // '*' | '/'
    Expression* left;
    Expression* right;
} Factor;

Factor factorCreate(char op, Expression* left, Expression* right) {
    Factor ret;
    ret.op = op;
    ret.left = left;
    ret.right = right;
    return ret;
}

typedef struct Term {
    char op; // '+' | '-'
    Expression *left;
    Expression *right;
} Term;

Term termCreate(char op, Expression* left, Expression* right) {
    Term ret;
    ret.op = op;
    ret.left = left;
    ret.right = right;
    return ret;
}

typedef struct Grouping {
    Expression *expression;
} Grouping;

Grouping groupingCreate(Expression* expression) {
    Grouping ret;
    ret.expression = expression;

    return ret;
}