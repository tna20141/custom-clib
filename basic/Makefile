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

CUSTOMIO_FILES = $(SRC_DIR)/customio.c $(INC_DIR)/customio.h $(TEST_DIR)/customio_test.c

$(BIN_DIR)/customio_test : $(CUSTOMIO_FILES) $(CMOCKA_SRC) $(CMOCKA_HEADERS)
	$(CC) $(CMOCKA_CCFLAGS) $(SRC_DIR)/customio.c $(CMOCKA_SRC) $(TEST_DIR)/customio_test.c \
		$(CUSTOMIO_CCFLAGS) -I $(INC_DIR) -o $@

customio_test : $(BIN_DIR)/customio_test
	$(BIN_DIR)/customio_test

# basic list test

LIST_CCFLAGS =

LIST_FILES = $(INC_DIR)/basic_general.h $(INC_DIR)/basic_list.h $(TEST_DIR)/basic_list_test.c

$(BIN_DIR)/basic_list_test : $(LIST_FILES) $(CMOCKA_SRC) $(CMOCKA_HEADERS)
	$(CC) $(CMOCKA_CCFLAGS) $(CMOCKA_SRC) $(TEST_DIR)/basic_list_test.c \
		$(LIST_CCFLAGS) -I $(INC_DIR) -o $@

basic_list_test : $(BIN_DIR)/basic_list_test
	$(BIN_DIR)/basic_list_test

#
# end of Cmocka test
#
