CPP       = g++
OBJ       = gtest_main.o dift_unittest.o $(GTEST_LIB)
LINKOBJ   = gtest_main.o dift_unittest.o $(GTEST_LIB)
LINKFLAGS = -lpthread -lgcov 
BIN_DEST  = bin
BIN       = $(BIN_DEST)/test_dift
CXXFLAGS  = -Wall -DCONFIG_DIFT_TEST -isystem ${GTEST_DIR}/include -isystem ${GMOCK_DIR}/include
GCOV_FILE = *.gcno *.gcda *.gcov

.PHONY: all clean

all: $(BIN)

clean:
	rm -rf $(OBJ) $(BIN_DEST) $(GCOV_FILE)

$(BIN): $(OBJ)
	mkdir -p $(BIN_DEST)
	$(CPP) $(LINKOBJ) -o $(BIN) $(LIBS) $(LINKFLAGS)

gtest_main.o: gtest_main.cc
	$(CPP) -c gtest_main.cc -o gtest_main.o $(CXXFLAGS)

dift_unittest.o: dift_unittest.cc
	$(CPP) -c dift_unittest.cc -o dift_unittest.o $(CXXFLAGS)
