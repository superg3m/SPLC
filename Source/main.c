#include <ckit.h>
#include <lexer.h>
#include <token.h>
#include <spl_parser.h>

int main(int argc, char** argv) {
    ckit_init();

    if (argc < 2) {
        LOG_ERROR("Usage: splc.exe <filename> \n");
        return -1;
    }

    char* file_path = argv[1];

    if (!ckit_os_path_exists(file_path)) {
        LOG_DEBUG("%s\n", ckit_os_get_cwd());
        LOG_ERROR("File name is not a valid path: %s\n", file_path);
        return -1;
    }

    // lexical analysis
    Lexer lexer = lexerCreate(); // later on I should make an arena version of this where it can just accept a arena allocator
    SPL_Token* token_stream = lexerGenerateTokenStream(&lexer, file_path);
    LOG_PRINT("\n");
    LOG_SUCCESS("------------ LEXICAL ANALYSIS ------------\n");

    for (int i = 0; i < ckit_vector_count(token_stream); i++ ) {
        SPL_Token token = token_stream[i];
        LOG_PRINT("%s(%s) | Line: %d\n", tokenTypeToString(token.type), token.lexeme, token.line);
    }
    lexerFree(&lexer);

    // parse
    LOG_PRINT("\n");
    LOG_SUCCESS("------------ PARSING ------------\n");
    Parser parser = parserCreate();
    Expression* ast = generateAST(&parser, token_stream);
    if (ast) {
        expressionPrint(ast);
    }


    ckit_vector_free(token_stream);
    parserFree(&parser);
    ckit_cleanup();
}