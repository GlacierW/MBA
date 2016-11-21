CPP       = g++
OBJ	  = gtest_main.o agent_unittest.o $(GTEST_LIB)
LINKOBJ	  = gtest_main.o agent_unittest.o $(GTEST_LIB)
LINKFLAGS = -lpthread -lgcov
BIN_DEST  = bin
BIN       = $(BIN_DEST)/test_agent
CXXFLAGS  = -Wall -DCONFIG_AGENT_TEST -isystem ${GTEST_DIR}/include -isystem ${GMOCK_DIR}/include
GCOV_FILE = *.gcno *.gcda *.gcov

.PHONY : all clean

all: $(BIN)

clean:
	rm -rf $(OBJ) $(BIN_DEST) $(GCOV_FILE)

$(BIN): $(OBJ)
	mkdir -p $(BIN_DEST)
	$(CPP) $(LINKOBJ) -o $(BIN) $(LIBS) $(LINKFLAGS)

gtest_main.o: gtest_main.cc
	$(CPP) -c gtest_main.cc -o gtest_main.o $(CXXFLAGS)

agent_unittest.o: agent_unittest.cc ../agent.c test.h
	$(CPP) -c agent_unittest.cc -o agent_unittest.o $(CXXFLAGS)
