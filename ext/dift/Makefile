CPP       = g++
CC        = gcc
OBJ       = gtest_main.o dift.o dift_unittest.o
LINKOBJ   = gtest_main.o dift.o dift_unittest.o
LINKFLAGS = -lgtest -lpthread -lgcov
BIN       = gtest_dift
CXXFLAGS  = -Wall -fprofile-arcs -ftest-coverage
CFLAGS    = -Wall -fprofile-arcs -ftest-coverage
RM        = rm -f
GCOV_FILE = *.gcno *.gcda *.gcov

.PHONY: 

all: $(BIN)

clean: clean-custom
	${RM} $(OBJ) $(BIN) $(GCOV_FILE)

$(BIN): $(OBJ)
	$(CPP) $(LINKOBJ) -o $(BIN) $(LIBS) $(LINKFLAGS)

gtest_main.o: gtest_main.cc
	$(CPP) -c gtest_main.cc -o gtest_main.o $(CXXFLAGS)

dift.o: dift.c
	$(CPP) -c dift.c -o dift.o $(CXXFLAGS)

dift_unittest.o: dift_unittest.cc
	$(CPP) -c dift_unittest.cc -o dift_unittest.o $(CXXFLAGS)
