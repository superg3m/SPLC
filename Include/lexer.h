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

    Token* tokens;
} Lexer;

Lexer lexer_create();
Token* lexer_generate_token_stream(Lexer* lexer, char* file_path);