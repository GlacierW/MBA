CPP       = g++
OBJ       = gtest_main.o memfrs_unittest.o $(GTEST_LIB) libjson-c.a
LINKOBJ   = gtest_main.o memfrs_unittest.o $(GTEST_LIB) libjson-c.a
LINKFLAGS = -lpthread -lgcov 
BIN_DEST  = bin
BIN       = $(BIN_DEST)/test_memfrs
CXXFLAGS  = -Wall -DCONFIG_MEMFRS_TEST -isystem ${GTEST_DIR}/include -isystem ${GMOCK_DIR}/include -g
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

memfrs_unittest.o:  libjson-c.a ../memfrs.c memfrs_unittest.cc
	$(CPP) -c memfrs_unittest.cc -o memfrs_unittest.o $(CXXFLAGS)

libjson-c.a:
	cd ../../../ && cd json-c && pwd && make
	cp ../../../json-c/.libs/libjson-c.a .
