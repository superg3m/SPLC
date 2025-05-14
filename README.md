# SPLC (Simple Programming Language Compiler/Interpreter)

SPLC is a small, educational interpreter written in pure C. It implements a simple programming language (SPL) with basic constructs like variables, arithmetic operations, conditional statements (if, else), loops (while, for), and print statements. The project is built from scratch with a focus on learning.

**GitHub**: [https://github.com/superg3m/SPLC](https://github.com/superg3m/SPLC)

## Features
- **Lexer & Parser**: The lexer tokenizes the source code, and the parser converts tokens into an abstract syntax tree (AST) that represents the structure of the program.
- **Interpreter**: The interpreter evaluates the AST and executes SPL programs.
- **Control Flow**: Supports conditional statements (if/else) and loops (while, for).
- **JSON AST Visualization**: Uses the cj library to visualize the AST in JSON format for debugging and better understanding.

## Dependencies
- **cj**: A minimal JSON library for visualizing the AST.
- **ckg**: A personal C library that provides low-level utilities like string handling, memory management, file I/O, and more. (Note: File I/O does not handle UTF-8 and is intended for educational purposes)
- **c_build**: A simple build system I created. However, a Makefile is included for convenience.

## Building

### Windows
```
git clone https://github.com/superg3m/SPLC.git ; cd SPLC

./bootstrap.ps1    # Only needs to be run once
./build.ps1 ; ./run.ps1
```

### Linux
```bash
git clone https://github.com/superg3m/SPLC.git ; cd SPLC


chmod +x bootstrap.sh build.sh run.sh

./bootstrap.sh
./build.sh
./run.sh

or 

git clone https://github.com/superg3m/SPLC.git ; cd SPLC
make

./make_build/splc.exe ./SPL_Source/test.spl
```

### Simple compiler version
```
mkdir make_build
gcc -std=c11 -Wall -Wno-deprecated -Wno-parentheses -Wno-missing-braces `
    -Wno-switch -Wno-unused-variable -Wno-unused-result -Werror -g `
    -I./Include -I./external_source `
    ./Source/ast.c `
    ./Source/expression.c `
    ./Source/interpreter.c `
    ./Source/lexer.c `
    ./Source/main.c `
    ./Source/spl_parser.c `
    ./Source/statement.c `
    ./Source/token.c `
    ./external_source/ckg.c `
    ./external_source/cj.c `
    -o make_build/splc.exe

./make_build/splc.exe ./SPL_Source/test.spl
```

## Example SPL
```py
x = (22 + 1) % 3;
x = x - 1;
print(23);
print("X's value is: " + x);
print("Hello World! | " + (4.32 + 54));

if (true) {
    y = 7 + x;
    x = y;
    print(x);
}

if (x == 8) {
    x = 5;
    print("X's value is: " + x);
    print("Hello: " + (3 * 4));
    print("World: " + (32 / 2));
} else {
    print(3 / 2);
    print(4);
    print("WOWS: " + y);
}

i = 0;
while (i < 5) {
    print("WOW THIS ACTUALLY WORKS: " +  i);
    i = i + 1;
}

for (j = 9; j >= x; j = j - 1) {
    print("Ogga: " + 3);
}
```

``` 
23
"X's value is: 1"
"Hello World! | 58.320000"
8
"X's value is: 5"
"Hello: 12"
"World: 16"
"WOW THIS ACTUALLY WORKS: 0"
"WOW THIS ACTUALLY WORKS: 1"
"WOW THIS ACTUALLY WORKS: 2"
"WOW THIS ACTUALLY WORKS: 3"
"WOW THIS ACTUALLY WORKS: 4"
"Ogga: 3"
"Ogga: 3"
"Ogga: 3"
"Ogga: 3"
"Ogga: 3"
```

## Project Structure
- Lexer: Tokenizes the input source code.
- Parser: Builds the abstract syntax tree (AST) from the tokens.
- Interpreter: Executes the AST, performing operations like arithmetic, binops, variable assignments, control flow, ect.

## Notes
- The project is mainly for learning purposes and may contain some experimental or "hacky" code. However, I am happy with the core design and am open to any feedback on the structure, code style, or interpreter design.