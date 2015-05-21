#include "dift.c"
#include <gtest/gtest.h>

// setup the global variable
phys_ram_base = 0x0;
phys_ram_size = 0x40000000;

TEST( DiftUnitTest, DIFT_INITILIZATION ) {}
TEST( DiftUnitTest, RECORD_ENQUEUE ) {}
TEST( DiftUnitTest, RECORD_ENQUEUE_ACROSS_CHUNK ) {}
TEST( DiftUnitTest, RECORD_DEQUEUE ) {}
TEST( DiftUnitTest, RECORD_DEQUEUE_ACROSS_CHUNK ) {}

TEST( DiftUnitTest, TCG_RECORD_ENQUEUE ) {}
TEST( DiftUnitTest, TCG_RECORD_ENQUEUE_ACROSS_CHUNK ) {}
TEST( DiftUnitTest, TCG_RECORD_DEQUEUE ) {}
TEST( DiftUnitTest, TCG_RECORD_DEQUEUE_ACROSS_CHUNK ) {}

TEST( DiftUnitTest, TCG_WADDR_ENQUEUE ) {}
TEST( DiftUnitTest, TCG_RADDR_ENQUEUE ) {}

TEST( DiftUnitTest, KICK_ENQUEUE_PTR ) {}
