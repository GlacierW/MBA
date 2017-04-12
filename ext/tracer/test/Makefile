CPP       = g++
OBJ       = gtest_main.o tracer_unittest.o $(GTEST_LIB)
LINKOBJ   = gtest_main.o tracer_unittest.o $(GTEST_LIB)
LINKFLAGS = -lpthread -lgcov 
BIN_DEST  = bin
BIN       = $(BIN_DEST)/test_tracer
CXXFLAGS  = -Wall -DCONFIG_TRACER_TEST -isystem ${GTEST_DIR}/include -isystem ${GMOCK_DIR}/include -g
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

tracer_unittest.o:  ../tracer.c tracer_unittest.cc
	$(CPP) -c tracer_unittest.cc -o tracer_unittest.o $(CXXFLAGS)
