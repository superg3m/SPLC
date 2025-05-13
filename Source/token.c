#include <token.h>

void token_print(SPL_Token token) {
    CKG_LOG_PRINT("%s(%.*s) | line: %d\n", token_strings[token.type], (int)token.name.length, token.name.data, token.line);
}

SPL_TokenType token_get_keyword(char* str, u64 str_length) {
    if (ckg_str_equal(str, str_length, CKG_LIT_ARG("if"))) {
        return SPL_TOKEN_IF;
    } else if (ckg_str_equal(str, str_length, CKG_LIT_ARG("else"))) {
        return SPL_TOKEN_ELSE;
    } else if (ckg_str_equal(str, str_length, CKG_LIT_ARG("for"))) {
        return SPL_TOKEN_FOR;
    } else if (ckg_str_equal(str, str_length, CKG_LIT_ARG("while"))) {
        return SPL_TOKEN_WHILE;
    } else if (ckg_str_equal(str, str_length, CKG_LIT_ARG("print"))) {
        return SPL_TOKEN_PRINT;
    } else if (ckg_str_equal(str, str_length, CKG_LIT_ARG("true"))) {
        return SPL_TOKEN_TRUE;
    } else if (ckg_str_equal(str, str_length, CKG_LIT_ARG("false"))) {
        return SPL_TOKEN_FALSE;
    } else if (ckg_str_equal(str, str_length, CKG_LIT_ARG("null"))) {
        return SPL_TOKEN_NULL;
    } else if (ckg_str_equal(str, str_length, CKG_LIT_ARG("return"))) {
        return SPL_TOKEN_RETURN;
    }

    return SPL_TOKEN_ILLEGAL_TOKEN;
}

// REALISTICALLY THIS SHOULD BE A HASHTABLE...
SPL_TokenType token_get_syntax(char* str, u64 str_length) {
    if (ckg_str_equal(str, str_length, CKG_LIT_ARG("="))) {
        return SPL_TOKEN_ASSIGNMENT;
    } else if (ckg_str_equal(str, str_length, CKG_LIT_ARG("%"))) {
        return SPL_TOKEN_MODULUS;
    } else if (ckg_str_equal(str, str_length, CKG_LIT_ARG("~"))) {
        return SPL_TOKEN_BITWISE_NOT;
    } else if (ckg_str_equal(str, str_length, CKG_LIT_ARG("+"))) {
        return SPL_TOKEN_PLUS;
    } else if (ckg_str_equal(str, str_length, CKG_LIT_ARG("-"))) {
        return SPL_TOKEN_MINUS;
    } else if (ckg_str_equal(str, str_length, CKG_LIT_ARG("/"))) {
        return SPL_TOKEN_DIVISION;
    } else if (ckg_str_equal(str, str_length, CKG_LIT_ARG("*"))) {
        return SPL_TOKEN_STAR;
    } else if (ckg_str_equal(str, str_length, CKG_LIT_ARG("&"))) {
        return SPL_TOKEN_AMPERSAND;
    } else if (ckg_str_equal(str, str_length, CKG_LIT_ARG("|"))) {
        return SPL_TOKEN_BITWISE_OR;
    } else if (ckg_str_equal(str, str_length, CKG_LIT_ARG("<"))) {
        return SPL_TOKEN_LESS_THAN;
    } else if (ckg_str_equal(str, str_length, CKG_LIT_ARG(">"))) {
        return SPL_TOKEN_GREATER_THAN;
    } else if (ckg_str_equal(str, str_length, CKG_LIT_ARG("!"))) {
        return SPL_TOKEN_NOT;
    } else if (ckg_str_equal(str, str_length, CKG_LIT_ARG("("))) {
        return SPL_TOKEN_LEFT_PAREN;
    } else if (ckg_str_equal(str, str_length, CKG_LIT_ARG(")"))) {
        return SPL_TOKEN_RIGHT_PAREN;
    } else if (ckg_str_equal(str, str_length, CKG_LIT_ARG(","))) {
        return SPL_TOKEN_COMMA;
    } else if (ckg_str_equal(str, str_length, CKG_LIT_ARG("."))) {
        return SPL_TOKEN_DOT;
    } else if (ckg_str_equal(str, str_length, CKG_LIT_ARG(";"))) {
        return SPL_TOKEN_SEMI_COLON;
    } else if (ckg_str_equal(str, str_length, CKG_LIT_ARG("["))) {
        return SPL_TOKEN_LEFT_BRACKET;
    } else if (ckg_str_equal(str, str_length, CKG_LIT_ARG("]"))) {
        return SPL_TOKEN_RIGHT_BRACKET;
    } else if (ckg_str_equal(str, str_length, CKG_LIT_ARG("{"))) {
        return SPL_TOKEN_LEFT_CURLY;
    } else if (ckg_str_equal(str, str_length, CKG_LIT_ARG("}"))) {
        return SPL_TOKEN_RIGHT_CURLY;
    } else if (ckg_str_equal(str, str_length, CKG_LIT_ARG("++"))) {
        return SPL_TOKEN_INCREMENT;
    } else if (ckg_str_equal(str, str_length, CKG_LIT_ARG("--"))) {
        return SPL_TOKEN_DECREMENT;
    } else if (ckg_str_equal(str, str_length, CKG_LIT_ARG("=="))) {
        return SPL_TOKEN_EQUALS;
    } else if (ckg_str_equal(str, str_length, CKG_LIT_ARG("!="))) {
        return SPL_TOKEN_NOT_EQUALS;
    } else if (ckg_str_equal(str, str_length, CKG_LIT_ARG("+="))) {
        return SPL_TOKEN_PLUS_EQUALS;
    } else if (ckg_str_equal(str, str_length, CKG_LIT_ARG("-="))) {
        return SPL_TOKEN_MINUS_EQUALS;
    } else if (ckg_str_equal(str, str_length, CKG_LIT_ARG("*="))) {
        return SPL_TOKEN_MULTIPLICATION_EQUALS;
    } else if (ckg_str_equal(str, str_length, CKG_LIT_ARG("/="))) {
        return SPL_TOKEN_DIVISION_EQUALS;
    } else if (ckg_str_equal(str, str_length, CKG_LIT_ARG(">="))) {
        return SPL_TOKEN_GREATER_THAN_EQUALS;
    } else if (ckg_str_equal(str, str_length, CKG_LIT_ARG("<="))) {
        return SPL_TOKEN_LESS_THAN_EQUALS;
    } else if (ckg_str_equal(str, str_length, CKG_LIT_ARG("<<"))) {
        return SPL_TOKEN_BITWISE_LEFT_SHIFT;
    } else if (ckg_str_equal(str, str_length, CKG_LIT_ARG(">>"))) {
        return SPL_TOKEN_BITWISE_RIGHT_SHIFT;
    } else if (ckg_str_equal(str, str_length, CKG_LIT_ARG("<<="))) {
        return SPL_TOKEN_BITWISE_LEFT_SHIFT_EQUALS;
    } else if (ckg_str_equal(str, str_length, CKG_LIT_ARG(">>="))) {
        return SPL_TOKEN_BITWISE_RIGHT_SHIFT_EQUALS;
    } else if (ckg_str_equal(str, str_length, CKG_LIT_ARG("---"))) {
        return SPL_TOKEN_UNINITIALIZED;
    } else if (ckg_str_equal(str, str_length, CKG_LIT_ARG("[..]"))) {
        return SPL_TOKEN_DYNAMIC_ARRAY;
    } else if (ckg_str_equal(str, str_length, CKG_LIT_ARG("&&"))) {
        return SPL_TOKEN_AND;
    } else if (ckg_str_equal(str, str_length, CKG_LIT_ARG("||"))) {
        return SPL_TOKEN_OR;
    } else if (ckg_str_equal(str, str_length, CKG_LIT_ARG("//"))) {
        return SPL_TOKEN_COMMENT;
    }
        
    return SPL_TOKEN_ILLEGAL_TOKEN;
}