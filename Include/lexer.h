#pragma once
#include <ckit.h>
#include <token.h>

typedef struct Lexer {
    u64 left_pos;
    u64 right_pos;
    u64 line;

    u8 c;
    u8* source;
    u64 source_size;

    CKIT_HashMap* syntaxTable;
    CKIT_HashMap* keywordTable;
    CKIT_HashSet* directiveTable;
    CKIT_HashSet* codeGenTable;
    CKIT_HashSet* primitiveTable;

    SPL_Token* tokens;
} Lexer;

Lexer lexerCreate();
void lexerFree(Lexer* lexer);
SPL_Token* lexerGenerateTokenStream(Lexer* lexer, char* file_path);