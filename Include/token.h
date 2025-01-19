#pragma once
#include <ckit.h>

typedef enum SPL_TokenType {
    SPL_TOKEN_ILLEGAL_TOKEN,
    SPL_TOKEN_EOF,
    SPL_TOKEN_NOT,                        // "!"
    SPL_TOKEN_GENERIC,                    // "$"
    SPL_TOKEN_BITWISE_AND,                // "&"
    SPL_TOKEN_LEFT_PAREN,                 // "("
    SPL_TOKEN_RIGHT_PAREN,                // ")"
    SPL_TOKEN_STAR,                       // "*"
    SPL_TOKEN_PLUS,                       // "+"
    SPL_TOKEN_COMMA,                      // ","
    SPL_TOKEN_MINUS,                      // "-"
    SPL_TOKEN_DOT,                        // "."
    SPL_TOKEN_DIVISION,                   // "/"
    SPL_TOKEN_SEMI_COLON,                 // ";"
    SPL_TOKEN_LESS_THAN,                  // "<"
    SPL_TOKEN_ASSIGNMENT,                 // "="
    SPL_TOKEN_GREATER_THAN,               // ">"
    SPL_TOKEN_LEFT_BRACKET,               // "["
    SPL_TOKEN_RIGHT_BRACKET,              // "]"
    SPL_TOKEN_LEFT_CURLY,                 // "{"
    SPL_TOKEN_BITWISE_OR,                 // "|"
    SPL_TOKEN_RIGHT_CURLY,                // "}"
    SPL_TOKEN_MODULUS,                    // "%"
    SPL_TOKEN_COLON,                      // ":"
    SPL_TOKEN_INCREMENT,                  // "++"
    SPL_TOKEN_DECREMENT,                  // "--"
    SPL_TOKEN_EQUALS,                     // "=="
    SPL_TOKEN_NOT_EQUALS,                 // "!="
    SPL_TOKEN_PLUS_EQUALS,                // "+="
    SPL_TOKEN_MINUS_EQUALS,               // "-="
    SPL_TOKEN_MULTIPLICATION_EQUALS,      // "*="
    SPL_TOKEN_DIVISION_EQUALS,            // "/="
    SPL_TOKEN_GREATER_THAN_EQUALS,        // ">="
    SPL_TOKEN_LESS_THAN_EQUALS,           // "<="
    SPL_TOKEN_BITWISE_LEFT_SHIFT,         // "<<"
    SPL_TOKEN_BITWISE_RIGHT_SHIFT,        // ">>"
    SPL_TOKEN_BITWISE_LEFT_SHIFT_EQUALS,  // "<<="
    SPL_TOKEN_BITWISE_RIGHT_SHIFT_EQUALS, // ">>="
    SPL_TOKEN_NOT_INITIALIZED,            // "---"
    SPL_TOKEN_DYNAMIC_ARRAY,              // "[..]"
    SPL_TOKEN_AND,                        // "&&"
    SPL_TOKEN_OR,                         // "||"
    SPL_TOKEN_COMMENT,                    // "//"
    SPL_TOKEN_IDENTIFIER,                 // any_word
    SPL_TOKEN_STRING_LITERAL,             // "TESTING
    SPL_TOKEN_INTEGER_LITERAL,            // 6
    SPL_TOKEN_FLOAT_LITERAL,              // 2.523
    SPL_TOKEN_CHARACTER_LITERAL,          // 'a'
    SPL_TOKEN_CODE_GEN,                   // @insert_if()
    SPL_TOKEN_DIRECTIVE,                  // #include
    SPL_TOKEN_PRIMITIVE_TYPE,             // u8
    SPL_TOKEN_STRUCT,                     // struct
    SPL_TOKEN_STRUCT_INTERFACE,           // struct_interface
    SPL_TOKEN_STRUCT_IMPL,                // struct_impl
    SPL_TOKEN_UNION,                      // union
    SPL_TOKEN_STATIC,                     // static
    SPL_TOKEN_EXTERN,                     // extern
    SPL_TOKEN_DEFER,                      // defer
    SPL_TOKEN_IF,                         // if
    SPL_TOKEN_ELSE,                       // else
    SPL_TOKEN_FOR,                        // for
    SPL_TOKEN_WHILE,                      // while
    SPL_TOKEN_TRUE,                       // true
    SPL_TOKEN_FALSE,                      // false
    SPL_TOKEN_NULL,                       // null
    SPL_TOKEN_RETURN,                      // return
    SPL_TOKEN_COUNT
} SPL_TokenType;

typedef struct Token {
    char *lexeme;
    SPL_TokenType type;
} Token;

const char* token_to_string(SPL_TokenType type);