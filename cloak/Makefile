# Makefile for custom-clib/cloak
# Currently this file contains rules for testing

# Program to compile C code.
CC = gcc

# Where to find headers.
INC_DIR = ./include

# Where to find user code.
SRC_DIR = ./src

# Where to find test code.
TEST_DIR = ./test

# Where to put the compiled files.
BIN_DIR = ./bin

# clean binary files
clean :
	rm -rf $(BIN_DIR)/*

#
# Cmocka test
#

# Cmocka root directory
CMOCKA_DIR = ../cmocka

# Cmocka sources
CMOCKA_SRC = $(CMOCKA_DIR)/src/cmocka.c

# Cmocka headers
CMOCKA_HEADERS = $(CMOCKA_DIR)/include/*.h

# Cmocka flags for compiler
CMOCKA_CCFLAGS = -isystem $(CMOCKA_DIR)/include -g

# Below are the rules for each cmocka test

# cloak test

CLOAK_CCFLAGS =

CLOAK_FILES = $(INC_DIR)/cloak_10.h $(INC_DIR)/cloak_template.h $(TEST_DIR)/cloak_test.c

$(BIN_DIR)/cloak_test : $(CLOAK_FILES) $(CMOCKA_SRC) $(CMOCKA_HEADERS)
	$(CC) $(CMOCKA_CCFLAGS) $(CMOCKA_SRC) $(TEST_DIR)/cloak_test.c \
		$(CLOAK_CCFLAGS) -I $(INC_DIR) -o $@

cloak_test : $(BIN_DIR)/cloak_test
	$(BIN_DIR)/cloak_test

#
# end of Cmocka test
#