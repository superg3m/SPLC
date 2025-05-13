#include <lexer.h>

Lexer lexer_create() {
    Lexer lexer = {0};
    return lexer;
}

internal bool lexer_is_EOF(Lexer* lexer) {
    return lexer->right_pos >= lexer->source.length;
}

internal void lexer_consume_next_char(Lexer* lexer) {
    lexer->c = lexer->source.data[lexer->right_pos];

    if (lexer->c == '\n') {
        lexer->line += 1;
    }

    lexer->right_pos += 1;
}

internal bool is_whitespace(char c) {
    return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}

internal bool lexer_consume_whitespace(Lexer* lexer) {
    if (is_whitespace(lexer->c)) {
        return true;
    }

    return false;
}

char lexer_peek_nth_char(Lexer* lexer, u64 n) {
    if ((lexer->right_pos + n) >= lexer->source.length) {
        return '\0';
    }

    return lexer->source.data[lexer->right_pos + n];
}

CKG_StringView lexer_get_scratch_buffer(Lexer* lexer) {
    return ckg_sv_create(lexer->source.data + lexer->left_pos, lexer->right_pos - lexer->left_pos);
}

void lexer_report_error(Lexer* lexer, char* msg) {
    CKG_StringView scratch = lexer_get_scratch_buffer(lexer);
    CKG_LOG_ERROR("String: %.*s\n", (int)scratch.length, scratch.data);
    CKG_LOG_ERROR("[LEXER ERROR] line: %d | %s", lexer->line, msg);
    ckg_assert(false);
}

bool lexer_consume_on_match(Lexer* lexer, char expected) {
    if (lexer_peek_nth_char(lexer, 0) != expected) {
        return false;
    }

    lexer_consume_next_char(lexer);
    return true;
}


void lexer_add_token(Lexer* lexer, SPL_TokenType token_type) {
    CKG_StringView name = lexer_get_scratch_buffer(lexer);
    ckg_vector_push(lexer->tokens, SPL_TOKEN_CREATE(token_type, name, lexer->line));
}

void lexer_consume_digit_literal(Lexer* lexer) {
    SPL_TokenType token_type = SPL_TOKEN_INTEGER_LITERAL;

    while (ckg_char_is_digit(lexer_peek_nth_char(lexer, 0)) || lexer_peek_nth_char(lexer, 0) == '.') {
        if (lexer->c == '.') {
            token_type = SPL_TOKEN_FLOAT_LITERAL;
        }

        lexer_consume_next_char(lexer);
    }

    lexer_add_token(lexer, token_type);
}

void lexer_consume_string_literal(Lexer* lexer) {
    while (lexer_peek_nth_char(lexer, 0) != '\"') {
        if (lexer_is_EOF(lexer)) {
            lexer_report_error(lexer, "String literal doesn't have a closing double quote!\n");
        }
        
        lexer_consume_next_char(lexer);
    }

    lexer_consume_next_char(lexer);
    lexer_add_token(lexer, SPL_TOKEN_STRING_LITERAL);
}

void lexer_consume_character_literal(Lexer* lexer) {
    if (lexer_consume_on_match(lexer, '\'')) {
        lexer_report_error(lexer, "character literal doesn't have any ascii data in between\n");
    }

    while (lexer_peek_nth_char(lexer, 0) != '\'') {
        if (lexer_is_EOF(lexer)) {
            lexer_report_error(lexer, "String literal doesn't have a closing double quote!\n");
        }
        
        lexer_consume_next_char(lexer);
    }

    lexer_consume_next_char(lexer);
    lexer_add_token(lexer, SPL_TOKEN_CHARACTER_LITERAL);
}

internal bool lexer_consume_literal(Lexer* lexer) {
    if (ckg_char_is_digit(lexer->c)) {
        lexer_consume_digit_literal(lexer);
        return true;
    } else if (lexer->c == '\"') {
        lexer_consume_string_literal(lexer);
        return true;
    } else if (lexer->c == '\'') {
        lexer_consume_character_literal(lexer);
        return true;
    }

    return false;
}


bool lexer_try_consume_word(Lexer* lexer) {
    if (!ckg_char_is_alpha(lexer->c)) {
        return false;
    }

    while (ckg_char_is_alpha_numeric(lexer_peek_nth_char(lexer, 0)) || lexer_peek_nth_char(lexer, 0) == '_') {
        if (lexer_is_EOF(lexer)) {
            break;
        }

        lexer_consume_next_char(lexer);
    }

    return true;
}

bool lexer_consume_word(Lexer* lexer) {
    if (!lexer_try_consume_word(lexer)) {
        return false;
    }

    CKG_StringView scratch = lexer_get_scratch_buffer(lexer);

    SPL_TokenType token_type = token_get_keyword(scratch.data, scratch.length);
    if (token_type != SPL_TOKEN_ILLEGAL_TOKEN) {
        lexer_add_token(lexer, token_type);
        return true;
    }

    lexer_add_token(lexer, SPL_TOKEN_IDENTIFIER);
    return true;
}

void lexer_consume_until_new_line(Lexer* lexer) {
    while (!lexer_is_EOF(lexer) && lexer_peek_nth_char(lexer, 0) != '\n') {
        lexer_consume_next_char(lexer);
    }
}

internal bool lexer_consume_syntax(Lexer* lexer) {
    if (lexer->c == '/') {
        if (lexer_consume_on_match(lexer, '/')) {
            lexer_consume_until_new_line(lexer);
            // lexer_add_token(lexer, SPL_TOKEN_COMMENT);
            return true;
        } else if (lexer_consume_on_match(lexer, '*')) {
            while (!(lexer_peek_nth_char(lexer, 0) == '*' && lexer_peek_nth_char(lexer, 1) == '/')) {
                if (lexer_is_EOF(lexer)) {
                    lexer_report_error(lexer, "Multiline comment doesn't terminate\n");
                }
                lexer_consume_next_char(lexer);
            }

            lexer_consume_next_char(lexer); // consume '*'
            lexer_consume_next_char(lexer); // consume '/'
            // lexer_add_token(lexer, SPL_TOKEN_COMMENT);
            return true;
        }
    } else if (lexer->c == '[') {
        if (lexer_peek_nth_char(lexer, 0) == '.' &&
            lexer_peek_nth_char(lexer, 1) == '.' &&
            lexer_peek_nth_char(lexer, 2) == ']') {
            lexer_consume_next_char(lexer); // consume '.'
            lexer_consume_next_char(lexer); // consume '.'
            lexer_consume_next_char(lexer); // consume ']'
        }
    } else if (lexer->c == '<') {
        lexer_consume_on_match(lexer, '<');
        lexer_consume_on_match(lexer, '=');
    } else if (lexer->c == '>') {
        lexer_consume_on_match(lexer, '>');
        lexer_consume_on_match(lexer, '=');
    } else if (lexer->c == '-') {
        if (!lexer_consume_on_match(lexer, '-')) {
            lexer_consume_on_match(lexer, '=');
        }
    } else if (lexer->c == '+') {
        if (!lexer_consume_on_match(lexer, '+')) {
            lexer_consume_on_match(lexer, '=');
        }
    } else if (lexer->c == '|') {
        if (!lexer_consume_on_match(lexer, '|')) {
            lexer_consume_on_match(lexer, '=');
        }
    } else if (lexer->c == '&') {
        if (!lexer_consume_on_match(lexer, '&')) {
            lexer_consume_on_match(lexer, '=');
        }
    } else if (lexer->c == '!' || lexer->c == '^' || lexer->c == '*' || lexer->c == '=') {
        lexer_consume_on_match(lexer, '=');
    }

    CKG_StringView buffer = lexer_get_scratch_buffer(lexer);
    SPL_TokenType token_type = token_get_syntax(buffer.data, buffer.length);
    if (token_type != SPL_TOKEN_ILLEGAL_TOKEN) {
        lexer_add_token(lexer, token_type);
        return true;
    }

    return false;
}

internal void lexer_consume_next_token(Lexer* lexer) {
    lexer->left_pos = lexer->right_pos;
    lexer_consume_next_char(lexer);

    if (lexer_consume_whitespace(lexer)) {}
    else if (lexer_consume_literal(lexer)) {}
    else if (lexer_consume_word(lexer)) {}
    else if (lexer_consume_syntax(lexer)) {}
    else {
        lexer_report_error(lexer, "Illegal token found\n");
    }
}

SPL_Token* lexer_consume_token_stream(Lexer* lexer, char* source, u64 source_length) {
    lexer->left_pos = 0;
    lexer->right_pos = 0;
    lexer->line = 1;
    lexer->tokens = NULL;
    lexer->c = '\0';

    lexer->source = ckg_sv_create(source, source_length);
    while (!lexer_is_EOF(lexer)) {
        lexer_consume_next_token(lexer);
    }

    ckg_vector_push(lexer->tokens, SPL_TOKEN_CREATE(SPL_TOKEN_EOF, CKG_SV_EMPTY(), lexer->line));

    return lexer->tokens;
}