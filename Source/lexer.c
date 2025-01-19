#include <lexer.h>

Lexer lexer_create() {
    Lexer lexer = {0};

    lexer.left_pos  = 0;
    lexer.right_pos = 0;
    lexer.line      = 1;
    lexer.c = '\0';
    lexer.source = NULLPTR;
    lexer.source_size = 0;

    return lexer;
}

void lexer_reset(Lexer* lexer) {
    lexer->left_pos  = 0;
    lexer->right_pos = 0;
    lexer->line      = 1;
    lexer->c = '\0';
    lexer->source = NULLPTR;
    lexer->source_size = 0;
}

internal Boolean lexer_isEOF(Lexer* lexer) {
    return lexer->right_pos >= lexer->source_size;
}

Token* lexer_generate_token_stream(Lexer* lexer, char* file_path) {
    u64 file_size = 0;
    u8* source_data = ckit_os_read_entire_file(file_path, &file_size);

    lexer->source = source_data;
    lexer->source_size = file_size;

    LOG_PRINT("%s\n", lexer->source);

    ckit_free(source_data);
    return NULLPTR;
}