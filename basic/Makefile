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

#
# Google Test stuff
#

# Points to the root of Google Test, relative to where this file is.
# Remember to tweak this if you move this file.
GTEST_DIR = ../gtest

# Flags passed to the preprocessor.
# Set Google Test's header directory as a system directory, such that
# the compiler doesn't generate warnings in Google Test headers.
CPPFLAGS += -isystem $(GTEST_DIR)/include

# Flags passed to the C++ compiler.
CXXFLAGS += -g -Wall -Wextra -pthread

# All Google Test headers.  Usually you shouldn't change this
# definition.
GTEST_HEADERS = $(GTEST_DIR)/include/gtest/*.h \
                $(GTEST_DIR)/include/gtest/internal/*.h

# House-keeping build targets.

clean :
	rm -f $(TESTS) gtest.a gtest_main.a *.o

# Builds gtest.a and gtest_main.a.

# Usually you shouldn't tweak such internal variables, indicated by a
# trailing _.
GTEST_SRCS_ = $(GTEST_DIR)/src/*.cc $(GTEST_DIR)/src/*.h $(GTEST_HEADERS)

# For simplicity and to avoid depending on Google Test's
# implementation details, the dependencies specified below are
# conservative and not optimized.  This is fine as Google Test
# compiles fast and for ordinary users its source rarely changes.
$(BIN_DIR)/gtest-all.o : $(GTEST_SRCS_)
	$(CXX) $(CPPFLAGS) -I$(GTEST_DIR) $(CXXFLAGS) -c \
            $(GTEST_DIR)/src/gtest-all.cc -o $@

$(BIN_DIR)/gtest_main.o : $(GTEST_SRCS_)
	$(CXX) $(CPPFLAGS) -I$(GTEST_DIR) $(CXXFLAGS) -c \
            $(GTEST_DIR)/src/gtest_main.cc -o $@

$(BIN_DIR)/gtest.a : $(BIN_DIR)/gtest-all.o
	$(AR) $(ARFLAGS) $@ $^

$(BIN_DIR)/gtest_main.a : $(BIN_DIR)/gtest-all.o $(BIN_DIR)/gtest_main.o
	$(AR) $(ARFLAGS) $@ $^

# Builds a sample test.  A test should link with either gtest.a or
# gtest_main.a, depending on whether it defines its own main()
# function.

# list test

$(BIN_DIR)/list_test.o : $(TEST_DIR)/list_test.cpp \
	$(INC_DIR)/list.h $(INC_DIR)/utils.h $(INC_DIR)/common_macros.h $(GTEST_HEADERS)
	$(CXX) $(CPPFLAGS) -I $(SRC_DIR) -I $(INC_DIR) $(CXXFLAGS) -c $(TEST_DIR)/list_test.cpp \
		-o $@

$(BIN_DIR)/list_test : $(BIN_DIR)/list_test.o $(BIN_DIR)/gtest_main.a
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -lpthread $^ -o $@

list_test : $(BIN_DIR)/list_test
	$(BIN_DIR)/list_test

#
# end of Google Test stuff
#

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

$(BIN_DIR)/customio_test : $(SRC_DIR)/customio.c $(INC_DIR)/customio.h $(TEST_DIR)/customio_test.c \
	$(CMOCKA_SRC) $(CMOCKA_HEADERS)
	$(CC) $(CMOCKA_CCFLAGS) $(SRC_DIR)/customio.c $(CMOCKA_SRC) $(TEST_DIR)/customio_test.c \
		$(CUSTOMIO_CCFLAGS) -I $(INC_DIR) -o $@

customio_test : $(BIN_DIR)/customio_test
	$(BIN_DIR)/customio_test

#
# end of Cmocka test
#
