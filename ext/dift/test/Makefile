CPP       = g++
CC        = gcc
OBJ       = gtest_main.o dift_unittest.o #dift.o
LINKOBJ   = gtest_main.o dift_unittest.o #dift.o
LINKFLAGS = -lpthread -lgcov -lgtest 
BIN       = gtest_dift
CXXFLAGS  = -Wall -DCONFIG_DIFT_GTEST
CFLAGS    = -Wall -DCONFIG_DIFT_GTEST
RM        = rm -f
GCOV_FILE = *.gcno *.gcda *.gcov

.PHONY: 

all: $(BIN)

clean:
	${RM} $(OBJ) $(BIN) $(GCOV_FILE)

$(BIN): $(OBJ)
	$(CPP) $(LINKOBJ) -o $(BIN) $(LIBS) $(LINKFLAGS)

gtest_main.o: gtest_main.cc
	$(CPP) -c gtest_main.cc -o gtest_main.o $(CXXFLAGS)

#dift.o: dift.c
#	$(CPP) -c dift.c -o dift.o $(CXXFLAGS)

dift_unittest.o: dift_unittest.cc
	$(CPP) -c dift_unittest.cc -o dift_unittest.o $(CXXFLAGS)
