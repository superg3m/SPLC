#pragma once
#include <ckg.h>
#include <token.h>

typedef struct Lexer {
    u64 left_pos;
    u64 right_pos;
    SPL_Token* tokens;
    u32 line;

    CKG_StringView source;
    char c;
} Lexer;

SPL_Token* consume_token_stream();