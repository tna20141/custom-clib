# Makefile for custom-clib/basic
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

# customio test

CUSTOMIO_CCFLAGS =

CUSTOMIO_SRCS = $(SRC_DIR)/customio.c

CUSTOMIO_HEADERS = $(INC_DIR)/customio.h

CUSTOMIO_FILES = $(CUSTOMIO_SRCS) $(CUSTOMIO_HEADERS) $(TEST_DIR)/customio_test.c

$(BIN_DIR)/customio_test : $(CUSTOMIO_FILES) $(CMOCKA_SRC) $(CMOCKA_HEADERS)
	$(CC) $(CMOCKA_CCFLAGS) $(CUSTOMIO_SRCS) $(CMOCKA_SRC) $(TEST_DIR)/customio_test.c \
		$(CUSTOMIO_CCFLAGS) -I $(INC_DIR) -o $@

customio_test : $(BIN_DIR)/customio_test
	$(BIN_DIR)/customio_test

# basic list test

LIST_CCFLAGS =

LIST_SRCS =

LIST_HEADERS = $(INC_DIR)/basic_general.h $(INC_DIR)/basic_list.h

LIST_FILES = $(LIST_HEADERS) $(TEST_DIR)/basic_list_test.c

$(BIN_DIR)/basic_list_test : $(LIST_FILES) $(CMOCKA_SRC) $(CMOCKA_HEADERS)
	$(CC) $(CMOCKA_CCFLAGS) $(LIST_SRCS) $(CMOCKA_SRC) $(TEST_DIR)/basic_list_test.c \
		$(LIST_CCFLAGS) -I $(INC_DIR) -o $@

basic_list_test : $(BIN_DIR)/basic_list_test
	$(BIN_DIR)/basic_list_test

# basic tree test

TREE_CCFLAGS =

TREE_SRCS = $(SRC_DIR)/basic_tree.c

TREE_HEADERS = $(INC_DIR)/basic_general.h $(INC_DIR)/basic_list.h $(INC_DIR)/basic_tree.h

TREE_FILES = $(TREE_SRCS) $(TREE_HEADERS) $(TEST_DIR)/basic_tree_test.c

$(BIN_DIR)/basic_tree_test : $(TREE_FILES) $(CMOCKA_SRC) $(CMOCKA_HEADERS)
	$(CC) $(CMOCKA_CCFLAGS) $(TREE_SRCS) $(CMOCKA_SRC) $(TEST_DIR)/basic_tree_test.c \
		$(TREE_CCFLAGS) -I $(INC_DIR) -o $@

basic_tree_test : $(BIN_DIR)/basic_tree_test
	$(BIN_DIR)/basic_tree_test

#
# end of Cmocka test
#
