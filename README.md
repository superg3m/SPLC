// https://github.com/NikRadi/minic

dependencies: [ckg, cj]

This has been tested with MSVC /fsanitize=address seems like there is not memory leaks or overwrites.

cj is used as my personal c library it has some neat stuff in there
cj is used for json parsing just for visualizing the ast

Lexer 
Parser
Interpreter

in the lexer probably put a notion of a SPL_TOKEN_SCOPE