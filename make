# Makefile for SPLC - call it `make_build`

# Compiler and flags
CC = gcc
CFLAGS = -std=c11 -Wall -Wno-deprecated -Wno-parentheses -Wno-missing-braces \
         -Wno-switch -Wno-unused-variable -Wno-unused-result -Werror -g

# Directories
SRC_DIR = ../../Source
EXT_DIR = ../../external_source
INC_DIRS = -I../../Include -I$(EXT_DIR)
OUT_DIR = ./build_gcc/debug
TARGET = $(OUT_DIR)/splc.exe

# Source files
SRCS = $(SRC_DIR)/ast.c \
       $(SRC_DIR)/expression.c \
       $(SRC_DIR)/interpreter.c \
       $(SRC_DIR)/lexer.c \
       $(SRC_DIR)/main.c \
       $(SRC_DIR)/spl_parser.c \
       $(SRC_DIR)/statement.c \
       $(SRC_DIR)/token.c \
       $(EXT_DIR)/ckg.c \
       $(EXT_DIR)/cj.c

# Default target
all: $(TARGET)

$(TARGET): $(SRCS)
	@mkdir -p $(OUT_DIR)
	$(CC) $(SRCS) $(CFLAGS) $(INC_DIRS) -o $(TARGET)

.PHONY: clean

clean:
       rm -f $(TARGET)