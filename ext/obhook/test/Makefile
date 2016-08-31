CPP       = g++
OBJ       = main.o obhook_unittest.o $(GTEST_LIB)
LINKOBJ   = main.o obhook_unittest.o $(GTEST_LIB)
LINKFLAGS = -lpthread -lgcov 
BIN_DEST  = bin
BIN       = $(BIN_DEST)/test_obhook
CXXFLAGS  = -Wall -DCONFIG_OBHOOK_TEST -isystem ${GTEST_DIR}/include -isystem ${GMOCK_DIR}/include
GCOV_FILE = *.gcno *.gcda *.gcov

.PHONY: all clean

all: $(BIN)

clean:
	rm -rf $(OBJ) $(BIN_DEST) $(GCOV_FILE)

$(BIN): $(OBJ)
	mkdir -p $(BIN_DEST)
	$(CPP) $(LINKOBJ) -o $(BIN) $(LIBS) $(LINKFLAGS)

main.o: main.cc
	$(CPP) -c main.cc -o main.o $(CXXFLAGS)

obhook_unittest.o: ../obhook.c obhook_unittest.cc
	$(CPP) -c obhook_unittest.cc -o obhook_unittest.o $(CXXFLAGS)
