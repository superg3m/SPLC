#pragma once
#include <ast.h>

typedef enum InterpreterType {
    INTERPRETER_RETURN_INVALID,
    INTERPRETER_NUMBER,
    INTERPRETER_STRING,
    INTERPRETER_BOOL,
    INTERPRETER_COUNT
} InterpreterType;

internal char* interpreter_type_strings[INTERPRETER_COUNT] = {
    stringify(INTERPRETER_RETURN_INVALID),
    stringify(INTERPRETER_NUMBER),
    stringify(INTERPRETER_STRING),
    stringify(INTERPRETER_BOOL)
};

typedef struct InterpreterReturn {
    InterpreterType type;
    union {
        CKG_StringView str;
        int i;
        double f;
        bool b;
    };
} InterpreterReturn;

#define INVALID_INTERPRETER_RETURN() (InterpreterReturn){INTERPRETER_RETURN_INVALID, 0}

typedef struct ScopeVar {
    CKG_StringView name;
    InterpreterReturn ret;
} ScopeVar;

typedef struct Scope {
    struct Scope* parent_scope;
    ScopeVar* variables;
} Scope;

void interpret_ast(ASTNode* ast);