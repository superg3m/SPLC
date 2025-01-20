#include <lexer.h>

void lexerInitalizeSyntaxTable(Lexer* lexer) {
    internal char* syntaxLookup[] = { 
        "!", "$", "&", "(", ")",
        "*", "+", ",", "-", ".", "/", ";",
        "<", "=", ">", "[", "]", "{", "|", "}",
        "%", ":", "++", "--", "==", "!=", "+=",
        "-=", "*=", "/=", ">=", "<=", "<<", ">>",
        "<<=", ">>=", "---", "[..]", "&&", "||", "//"
    };

    internal SPL_TokenType syntaxTokenTable[] = {
        SPL_TOKEN_NOT, SPL_TOKEN_GENERIC, SPL_TOKEN_BITWISE_AND, 
        SPL_TOKEN_LEFT_PAREN, SPL_TOKEN_RIGHT_PAREN, SPL_TOKEN_STAR, 
        SPL_TOKEN_PLUS, SPL_TOKEN_COMMA, SPL_TOKEN_MINUS, SPL_TOKEN_DOT, 
        SPL_TOKEN_DIVISION, SPL_TOKEN_SEMI_COLON, SPL_TOKEN_LESS_THAN, 
        SPL_TOKEN_ASSIGNMENT, SPL_TOKEN_GREATER_THAN, SPL_TOKEN_LEFT_BRACKET, 
        SPL_TOKEN_RIGHT_BRACKET, SPL_TOKEN_LEFT_CURLY, SPL_TOKEN_BITWISE_OR, 
        SPL_TOKEN_RIGHT_CURLY, SPL_TOKEN_MODULUS, SPL_TOKEN_COLON, 
        SPL_TOKEN_INCREMENT, SPL_TOKEN_DECREMENT, SPL_TOKEN_EQUALS, 
        SPL_TOKEN_NOT_EQUALS, SPL_TOKEN_PLUS_EQUALS, SPL_TOKEN_MINUS_EQUALS,
        SPL_TOKEN_MULTIPLICATION_EQUALS, SPL_TOKEN_DIVISION_EQUALS, SPL_TOKEN_GREATER_THAN_EQUALS, 
        SPL_TOKEN_LESS_THAN_EQUALS, SPL_TOKEN_BITWISE_LEFT_SHIFT, SPL_TOKEN_BITWISE_RIGHT_SHIFT, 
        SPL_TOKEN_BITWISE_LEFT_SHIFT_EQUALS, SPL_TOKEN_BITWISE_RIGHT_SHIFT_EQUALS, 
        SPL_TOKEN_NOT_INITIALIZED, SPL_TOKEN_DYNAMIC_ARRAY, 
        SPL_TOKEN_AND, SPL_TOKEN_OR, SPL_TOKEN_COMMENT
    };

    ckit_assert_msg(ArrayCount(syntaxTokenTable) == ArrayCount(syntaxLookup),"lookup tables should be the same count\n");

    for (int i = 0; i < ArrayCount(syntaxTokenTable); i++) {
        void* ret = ckit_hashmap_put(lexer->syntaxTable, syntaxLookup[i], &syntaxTokenTable[i]);
        ckit_assert_msg(ret == NULLPTR, "There should never be any duplicate keys\n");
    }
}

void lexerInitalizeKeywordTable(Lexer* lexer) {
     internal SPL_TokenType keywordTokenTable[] = {
        SPL_TOKEN_STRUCT, SPL_TOKEN_STRUCT_INTERFACE, SPL_TOKEN_STRUCT_IMPL,
        SPL_TOKEN_UNION, SPL_TOKEN_STATIC, SPL_TOKEN_EXTERN, SPL_TOKEN_DEFER,
        SPL_TOKEN_IF, SPL_TOKEN_ELSE, SPL_TOKEN_FOR, SPL_TOKEN_WHILE,
        SPL_TOKEN_TRUE, SPL_TOKEN_FALSE, SPL_TOKEN_NULL, SPL_TOKEN_RETURN
    };

    char* keywords[] = {
        "struct", "struct_interface", "struct_impl",
        "union", "static", "extern", "defer", "if", "else",
        "for", "while", "true", "false", "null", "return"
    };

    for (int i = 0; i < ArrayCount(keywords); i++) {
        void* ret = ckit_hashmap_put(lexer->keywordTable, keywords[i], &keywordTokenTable[i]);
        ckit_assert_msg(ret == NULLPTR, "There should never be any duplicate key\n");
    }
}

void lexerInitalizeDirectivesTable(Lexer* lexer) {
    char* directives[] = {
        "#include", "#if", "#elif",
        "#endif", "#ifdef", "#ifndef",
        "defined", "#define", "#assert"
    };

    for (int i = 0; i < ArrayCount(directives); i++) {
        ckit_hashset_put(lexer->directiveTable, directives[i]);
    }
}

void lexerInitalizeCodeGenTable(Lexer* lexer) {
    char* codeGen[] = {
        "@typeof", "@insert_if", "@global"
    };

    for (int i = 0; i < ArrayCount(codeGen); i++) {
        ckit_hashset_put(lexer->codeGenTable, codeGen[i]);
    }
}

void lexerInitalizePrimitivesTable(Lexer* lexer) {
    char* primitives[] = {
        "void", "bool","int", "char", "string",
        "u8", "u16", "u32", "u64", "s8", "s16",
        "s32", "s64", "float", "double"
    };

    for (int i = 0; i < ArrayCount(primitives); i++) {
        ckit_hashset_put(lexer->primitiveTable, primitives[i]);
    }
}


void lexerInitalizeTables(Lexer* lexer) {
    lexerInitalizeSyntaxTable(lexer);
    lexerInitalizeKeywordTable(lexer);
    lexerInitalizeDirectivesTable(lexer);
    lexerInitalizeCodeGenTable(lexer);
    lexerInitalizePrimitivesTable(lexer);
}

Lexer lexerCreate() {
    Lexer lexer = {0};

    lexer.left_pos  = 0;
    lexer.right_pos = 0;
    lexer.line      = 1;
    lexer.c = '\0';
    lexer.source = NULLPTR;
    lexer.source_size = 0;

    lexer.syntaxTable = ckit_hashmap_create(16, SPL_Token, FALSE);
    lexer.keywordTable = ckit_hashmap_create(16, SPL_Token, FALSE);
    lexer.directiveTable = ckit_hashset_create(16);
    lexer.codeGenTable = ckit_hashset_create(16);
    lexer.primitiveTable = ckit_hashset_create(16);

    lexerInitalizeTables(&lexer);

    return lexer;
}

void lexerFree(Lexer* lexer) {
    ckit_hashmap_free(lexer->syntaxTable);
    ckit_hashmap_free(lexer->keywordTable);
    ckit_hashset_free(lexer->directiveTable);
    ckit_hashset_free(lexer->codeGenTable);
    ckit_hashset_free(lexer->primitiveTable);
    ckit_vector_free(lexer->tokens);
}

internal void lexerReset(Lexer* lexer) {
    lexer->left_pos  = 0;
    lexer->right_pos = 0;
    lexer->line      = 1;
    lexer->c = '\0';
    lexer->source = NULLPTR;
    lexer->source_size = 0;
}

internal SPL_TokenType getAcceptedSyntax(Lexer* lexer, char* buffer) {
    SPL_TokenType* ret = ckit_hashmap_get(lexer->syntaxTable, buffer);

    if (ret) {
        return *ret;
    }

    return SPL_TOKEN_ILLEGAL_TOKEN;
}

internal String getScratchBuffer(Lexer* lexer) {
    return ckit_substring(lexer->source, lexer->left_pos, lexer->right_pos);
}

internal Boolean lexerIsEOF(Lexer* lexer) {
    return lexer->right_pos >= lexer->source_size;
}

internal char peekNthChar(Lexer* lexer, u8 n) {
    if ((lexer->right_pos + n) >= lexer->source_size) {
        return '\0';
    }

    return lexer->source[lexer->right_pos + n];
}

internal consumeNextChar(Lexer* lexer) {
    lexer->c = lexer->source[lexer->right_pos];
    lexer->right_pos += 1;
}

Boolean consumeOnMatch(Lexer* lexer, char expected) {
    if (peekNthChar(lexer, 0) != expected) {
        return FALSE;
    }

    consumeNextChar(lexer);
    return TRUE;
}

internal void consumeUntilNewLine(Lexer* lexer) {
    while (!lexerIsEOF(lexer) && peekNthChar(lexer, 0) != '\n') {
        consumeNextChar(lexer);
    }
}

internal void addToken(Lexer* lexer, SPL_TokenType type) {
    ckit_vector_push(lexer->tokens, tokenCreate(type, getScratchBuffer(lexer), lexer->line));
}

internal Boolean consumeSyntax(Lexer* lexer) {
    switch (lexer->c) {
        case '[': {
            if (consumeOnMatch(lexer, '.')) {
                ckit_assert_msg(consumeOnMatch(lexer, '.'), "Malformed Dynamic Array expected '.'\n");
                ckit_assert_msg(consumeOnMatch(lexer, ']'), "Malformed Dynamic Array ']'\n");
                addToken(lexer, SPL_TOKEN_DYNAMIC_ARRAY);
                return TRUE;
            }
        } break;

        case '/': {
            if (consumeOnMatch(lexer, '/')) {
                consumeUntilNewLine(lexer);
                addToken(lexer, SPL_TOKEN_COMMENT);
                return TRUE;
            }
        }

        case '<':
        case '>': {
            consumeOnMatch(lexer, lexer->c); // << or >>
            consumeOnMatch(lexer, '=');
        } break;


        // case '~':
        case '+':
        case '-': 
        case '|':
        case '&': {
            if (consumeOnMatch(lexer, lexer->c)) { // -- or ++ or && or ||
                // No Operation
            } else {
                consumeOnMatch(lexer, '=');
            }
        }

        case '!':
        case '%': {
            consumeOnMatch(lexer, '=');
        } break;
    }

    String buffer = getScratchBuffer(lexer);
    SPL_TokenType type = getAcceptedSyntax(lexer, buffer);
    if (type != SPL_TOKEN_ILLEGAL_TOKEN) {
        addToken(lexer, type);
        return TRUE;
    }

    return FALSE;
}

void reportError(Lexer* lexer, char* msg) {
    LOG_ERROR("Lexical Error:", getScratchBuffer(lexer));
    LOG_ERROR("Error Line: %d | %s", lexer->line, msg);
    ckit_assert(FALSE);
}


internal void consumeNextToken(Lexer* lexer) {
    lexer->left_pos = lexer->right_pos;
    consumeNextChar(lexer);

    if (consumeSyntax(lexer)) {}
    // else if (consumeSyntax(lexer)) {}
    // else if (consumeSyntax(lexer)) {}
    // else if (consumeSyntax(lexer)) {}
    // else if (consumeSyntax(lexer)) {}
    else {
        reportError(lexer, "Illegal token found");
    }
}


SPL_Token* lexerGenerateTokenStream(Lexer* lexer, char* file_path) {
    u64 file_size = 0;
    u8* source_data = ckit_os_read_entire_file(file_path, &file_size);

    lexer->source = source_data;
    lexer->source_size = file_size;

    consumeNextToken(lexer);
    LOG_SUCCESS("%s(%s)\n", tokenTypeToString(lexer->tokens[0].type), lexer->tokens[0].lexeme);
    /*
    while (!lexer_isEOF(lexer)) {
        
    }
    */
    ckit_free(source_data);
    return NULLPTR;
}