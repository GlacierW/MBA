#include <gtest/gtest.h>
#include "dift.c"


TEST( DiftUnitTest, DIFT_CONTEXT_INITILIZATION ) {
    
    // setup the global variable
    phys_ram_base = 0x0;
    phys_ram_size = 0x4000000;
    
    init_dift_context( dc );
    
    printf( "    dc->tail = %08x\n", dc->tail );
    ASSERT_EQ( 0, dc->tail );

    printf( "    dc->prev_tail = %08x\n", dc->prev_tail );
    ASSERT_EQ( Q_CHUNKS_SIZE - 1, dc->prev_tail );

    printf( "    Register dirty table check ... " );
    for( int i = 0; i < 24; ++i )
        for( int j = 0; j < 8; ++j )
            ASSERT_EQ( 0, dc->reg_dirty_tbl[i][j] );
    printf( "Done\n" );

    printf( "    Memory dirty table check ... " );
    for( int i = 0; i < phys_ram_size; ++i ) 
        ASSERT_EQ( 0, dc->mem_dirty_tbl[i] );
    printf("Done\n");

    printf( "    Disk dirty table check ... " );
    for( int i = 0; i < HD_L1_INDEX_BITS; ++i )
        ASSERT_EQ( NULL, dc->hd_l1_dirty_tbl[i] );
    printf( "Done\n" );
}

TEST( DiftUnitTest, DIFT_CASEMAP_INITIALIZATION ) {

    init_case_mapping();

    ASSERT_EQ( INSIDE_REG_ASSIGN, case_mapping[((0xc0 | OPT_REG << 4 | OPT_REG << 2 | MO_8) << 8) | EFFECT_INSIDE_REG | EFFECT_ASSIGN] );
    ASSERT_EQ( REG_REG_MIX_APPEND_MO16, case_mapping[((0xc0 | OPT_REG << 4 | OPT_REG << 2 | MO_16) << 8) | EFFECT_MIX | EFFECT_APPEND]);
    ASSERT_EQ( REG_MEM_OO_ASSIGN_MO32, case_mapping[((0xc0 | OPT_MEM << 4 | OPT_REG << 2 | MO_32) << 8) | EFFECT_ONE_TO_ONE | EFFECT_ASSIGN] );
    ASSERT_EQ( MEM_REG_OO_APPEND_MO64, case_mapping[((0xc0 | OPT_REG << 4 | OPT_MEM << 2 | MO_64) << 8) | EFFECT_ONE_TO_ONE | EFFECT_APPEND] );
}

TEST( DiftUnitTest, DIFT_QUEUE_INITIALIZATION ) {

    init_queue();

    ASSERT_EQ( 1, head );
    ASSERT_EQ( 0, prev_head );

    ASSERT_EQ( CHUNK_FILLING, q_chunks_flag[prev_head] ); 

    for( int i = 1; i < Q_CHUNKS_SIZE; ++i ) {
        ASSERT_EQ( CHUNK_AVAILABLE, q_chunks_flag[i] );
        ASSERT_EQ( &q_records[(i * SIZE_OF_CHUNK)/sizeof(uint64_t)], q_chunks_ptr[i] );
    }

    ASSERT_EQ( q_records, enqptr );
}

TEST( DiftUnitTest, DIFT_PREGEN_ROUTINE ) {
}

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
