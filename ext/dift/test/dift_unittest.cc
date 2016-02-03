/*
 *  De-coupled Information Flow Tracking (DIFT) test cases
 *
 *  Copyright (c) 2016 Chiawei Wang
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, see <http://www.gnu.org/licenses/>.
 */

#include <gtest/gtest.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

extern "C" {
#include "../dift.c"
}

static void dump_func_insn( const char* func_name, uint8_t* func_ptr ) {

    int fd = -1;
    int rt_sz;

    ASSERT_NE( -1, (fd = open("rt_dump", O_RDWR | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO)) );
    ftruncate( fd, 0 );

    printf( "\n=============== %s ==============\n", func_name );
    fflush( stdout );

    for( rt_sz = 512 - 1; rt_sz >=0 && func_ptr[rt_sz] != 0xc3; --rt_sz );
    write( fd, func_ptr, rt_sz + 1 ); // rt_sz as index, thereby + 1
    system( "ndisasm -b 64 rt_dump" );

    printf( "=================================\n" );
    fflush( stdout );

    unlink( "rt_dump" );
}

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
    for( int i = 0; i < (int)phys_ram_size; ++i ) 
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
    ASSERT_EQ( REG_REG_MIX_APPEND_MO16, case_mapping[((0xc0 | OPT_REG << 4 | OPT_REG << 2 | MO_16) << 8) | EFFECT_MIX | EFFECT_APPEND] );
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

#define dump_case_nb(name) \
    printf( #name ": %016x\n", case_list[name] )

TEST( DiftUnitTest, DIFT_DUMP_CASE ) {
    dump_case_nb(INSIDE_REG_ASSIGN);
    dump_case_nb(INSIDE_REG_APPEND);
    dump_case_nb(REG_REG_OO_ASSIGN_MO8);
    dump_case_nb(REG_REG_OO_ASSIGN_MO16);
    dump_case_nb(REG_REG_OO_ASSIGN_MO32);
    dump_case_nb(REG_REG_OO_ASSIGN_MO64);
    dump_case_nb(REG_REG_OO_APPEND_MO8);
    dump_case_nb(REG_REG_OO_APPEND_MO16);
    dump_case_nb(REG_REG_OO_APPEND_MO32);
    dump_case_nb(REG_REG_OO_APPEND_MO64);
    dump_case_nb(REG_REG_MIX_APPEND_MO8);
    dump_case_nb(REG_REG_MIX_APPEND_MO16);
    dump_case_nb(REG_REG_MIX_APPEND_MO32);
    dump_case_nb(REG_REG_MIX_APPEND_MO64);
    dump_case_nb(REG_MEM_OO_ASSIGN_MO8);
    dump_case_nb(REG_MEM_OO_ASSIGN_MO16);
    dump_case_nb(REG_MEM_OO_ASSIGN_MO32);
    dump_case_nb(REG_MEM_OO_ASSIGN_MO64);
    dump_case_nb(REG_MEM_OO_APPEND_MO8);
    dump_case_nb(REG_MEM_OO_APPEND_MO16);
    dump_case_nb(REG_MEM_OO_APPEND_MO32);
    dump_case_nb(REG_MEM_OO_APPEND_MO64);
    dump_case_nb(REG_MEM_MIX_ASSIGN_MO16);
    dump_case_nb(REG_MEM_MIX_ASSIGN_MO32);
    dump_case_nb(REG_MEM_MIX_ASSIGN_MO64);
    dump_case_nb(REG_MEM_MIX_APPEND_MO8);
    dump_case_nb(REG_MEM_MIX_APPEND_MO16);
    dump_case_nb(REG_MEM_MIX_APPEND_MO32);
    dump_case_nb(REG_MEM_MIX_APPEND_MO64);
    dump_case_nb(MEM_REG_OO_ASSIGN_MO8);
    dump_case_nb(MEM_REG_OO_ASSIGN_MO16);
    dump_case_nb(MEM_REG_OO_ASSIGN_MO32);
    dump_case_nb(MEM_REG_OO_ASSIGN_MO64);
    dump_case_nb(MEM_REG_OO_APPEND_MO8);
    dump_case_nb(MEM_REG_OO_APPEND_MO16);
    dump_case_nb(MEM_REG_OO_APPEND_MO32);
    dump_case_nb(MEM_REG_OO_APPEND_MO64);
    dump_case_nb(MEM_REG_MIX_ASSIGN_MO16);
    dump_case_nb(MEM_REG_MIX_ASSIGN_MO32);
    dump_case_nb(MEM_REG_MIX_ASSIGN_MO64);
    dump_case_nb(MEM_REG_MIX_APPEND_MO8);
    dump_case_nb(MEM_REG_MIX_APPEND_MO16);
    dump_case_nb(MEM_REG_MIX_APPEND_MO32);
    dump_case_nb(MEM_REG_MIX_APPEND_MO64);
    dump_case_nb(MEM_MEM_OO_ASSIGN_MO8);
    dump_case_nb(MEM_MEM_OO_ASSIGN_MO16);
    dump_case_nb(MEM_MEM_OO_ASSIGN_MO32);
    dump_case_nb(MEM_MEM_OO_ASSIGN_MO64);
    dump_case_nb(REG_IM_CLEAR_MO8);
    dump_case_nb(REG_IM_CLEAR_MO16);
    dump_case_nb(REG_IM_CLEAR_MO32);
    dump_case_nb(REG_IM_CLEAR_MO64);
    dump_case_nb(MEM_IM_CLEAR_MO8);
    dump_case_nb(MEM_IM_CLEAR_MO16);
    dump_case_nb(MEM_IM_CLEAR_MO32);
    dump_case_nb(MEM_IM_CLEAR_MO64);
    dump_case_nb(MEM_HD);
    dump_case_nb(HD_MEM);
}

TEST( DiftUnitTest, DIFT_PREGEN_ROUTINE ) {
    
    // This test require human recognition due to the ad-hoc string format mapping
    pre_generate_routine();   

    /*
    printf( "Information for routine check: \n" );
    printf( "&head  = 0x%16lx\n", &head );
    printf( "&enqptr  = 0x%16lx\n", &enqptr );
    printf( "q_chunks_flag[]  = 0x%16lx\n", q_chunks_flag );
    printf( "&last_mem_read_addr = 0x%16lx\n", &last_mem_read_addr );
    printf( "&last_mem_write_addr = 0x%16lx\n", &last_mem_write_addr );
    */
    
    const char* func_name[] = {
        "rt_finish_curr_block",
        "rt_get_next_enqptr",
        "rt_enqueue_one_rec",
        "rt_enqueue_raddr",
        "rt_enqueue_waddr"
    };

    uint8_t* func_ptr[] = {
        rt_finish_curr_block,
        rt_get_next_enqptr,
        rt_enqueue_one_rec,
        rt_enqueue_raddr,
        rt_enqueue_waddr
    };

    for( uint32_t i = 0; i < sizeof(func_name) / sizeof(char*); ++i )
        dump_func_insn( func_name[i], func_ptr[i] );
}

TEST( DiftUnitTest, DIFT_RECORD_ENQUEUE ) {

    uint64_t rec = 0x1234567812345678;

    EXPECT_EQ( 0, prev_head );
    EXPECT_EQ( 1, head );

    // single enqueue
    ASSERT_EQ( q_chunks_ptr[prev_head], enqptr );
    ASSERT_EQ( CHUNK_FILLING, q_chunks_flag[prev_head] );

    dift_rec_enqueue( rec );
    
    ASSERT_EQ( q_chunks_ptr[prev_head] + 1, enqptr );
    ASSERT_EQ( CHUNK_FILLING, q_chunks_flag[prev_head] );

    // cross 2 chunks enqueue
    uint64_t old_prev_head = prev_head;
    int enq_nb = ((SIZE_OF_CHUNK) / sizeof(uint64_t)) * 2;

    printf( "Number of records to enqueue = %d, chunks = %ld\n", enq_nb, (enq_nb * sizeof(uint64_t))/(SIZE_OF_CHUNK) );
    printf( "Before, prev_head = %ld, head = %ld\n", prev_head, head );
    for( int i = 0; i < enq_nb; ++i )
        dift_rec_enqueue( rec );
    printf( "After,  prev_head = %ld, head = %ld\n", prev_head, head );
    
    ASSERT_EQ( (old_prev_head + 2) % Q_CHUNKS_SIZE, prev_head );
    ASSERT_EQ( (prev_head + 1) % Q_CHUNKS_SIZE, head );
    ASSERT_EQ( CHUNK_FILLED, q_chunks_flag[old_prev_head] );
    ASSERT_EQ( q_chunks_ptr[prev_head] + 1, enqptr );
    ASSERT_EQ( CHUNK_FILLING, q_chunks_flag[prev_head] );
}

TEST( DiftUnitTest, DIFT_RECORD_DEQUEUE ) {
    
    int deq_nb = (SIZE_OF_CHUNK) / sizeof(uint64_t);
    
    uint64_t rec;
    uint64_t old_tail,
             old_prev_tail;

    // first dequeue
    rec = rec_dequeue( dc );

    EXPECT_EQ( 0, dc->prev_tail );
    EXPECT_EQ( 1, dc->tail );
    ASSERT_EQ( 0x1234567812345678, rec );
    ASSERT_EQ( CHUNK_CONSUMING, q_chunks_flag[dc->prev_tail] );

    old_prev_tail = dc->prev_tail;
    old_tail = dc->tail;
    for( int i = deq_nb - 1; i >= 0; --i )
        rec = rec_dequeue( dc );

    ASSERT_EQ( CHUNK_AVAILABLE, q_chunks_flag[old_prev_tail] );
    ASSERT_EQ( old_prev_tail + 1, dc->prev_tail );
    ASSERT_EQ( old_tail + 1, dc->tail );
}

TEST( DiftUnitTest, TCG_RECORD_ENQUEUE ) {
    
    // reset the record queue for TCG testing
    init_queue();
    dc->tail = 0;
    dc->prev_tail = Q_CHUNKS_SIZE - 1;
    dc->deqptr = NULL;
    
    uint64_t rec = 0x1234567812345678;

    uint64_t old_head,
             old_prev_head;

    EXPECT_EQ( 0, prev_head );
    EXPECT_EQ( 1, head );

    // single enqueue
    ASSERT_EQ( q_chunks_ptr[prev_head], enqptr );
    ASSERT_EQ( CHUNK_FILLING, q_chunks_flag[prev_head] );

    asm( "mov %0, %%rdx"
             : 
             : "r"(rec)
             : "%rdx" );

    asm( "call *%0" : : "r"(rt_enqueue_one_rec) );

    ASSERT_EQ( q_chunks_ptr[prev_head] + 1, enqptr );
    ASSERT_EQ( CHUNK_FILLING, q_chunks_flag[prev_head] );

    // cross 2 chunks enqueue
    old_head = head;
    old_prev_head = prev_head;

    int enq_nb = ((SIZE_OF_CHUNK) / sizeof(uint64_t)) * 2;
    for( int i = 0; i < enq_nb; ++i ) {
        asm( "mov %0, %%rdx"
             : 
             : "r"(rec)
             : "%rdx" );

        asm( "call *%0" : : "r"(rt_enqueue_one_rec) );
    }
    
    ASSERT_EQ( (old_prev_head + 2) % Q_CHUNKS_SIZE, prev_head );
    ASSERT_EQ( (old_head + 2) % Q_CHUNKS_SIZE, head );
    ASSERT_EQ( (prev_head + 1) % Q_CHUNKS_SIZE, head );
    ASSERT_EQ( CHUNK_FILLED, q_chunks_flag[old_prev_head] );
    ASSERT_EQ( q_chunks_ptr[prev_head] + 1, enqptr );
    ASSERT_EQ( CHUNK_FILLING, q_chunks_flag[prev_head] );
}

TEST( DiftUnitTest, TCG_RWADDR_ENQUEUE ) {

    volatile uint64_t* old_enqptr = enqptr;

    last_mem_read_addr = 0x4141414141414141;
    last_mem_write_addr = 0x4242424242424242;

    asm( "call *%0" : : "r"(rt_enqueue_raddr) );
    asm( "call *%0" : : "r"(rt_enqueue_waddr) );

    EXPECT_EQ( old_enqptr + 2, enqptr );
    ASSERT_EQ( 0x4141414141414141, *old_enqptr );
    ASSERT_EQ( 0x4242424242424242, *(old_enqptr + 1) );
}

TEST( DiftUnitTest, GET_DIFT_CASE ) {

    ASSERT_EQ( 
        REG_REG_OO_ASSIGN_MO32,
        dift_rec_case_nb(OPT_REG, OPT_REG, MO_32, EFFECT_ONE_TO_ONE | EFFECT_ASSIGN) );

    ASSERT_EQ( 
        REG_MEM_OO_APPEND_MO64,
        dift_rec_case_nb(OPT_REG, OPT_MEM, MO_64, EFFECT_ONE_TO_ONE | EFFECT_APPEND) );

    ASSERT_EQ( 
        MEM_REG_MIX_APPEND_MO16,
        dift_rec_case_nb(OPT_MEM, OPT_REG, MO_16, EFFECT_MIX | EFFECT_APPEND) );

    ASSERT_EQ( 
        MEM_MEM_OO_ASSIGN_MO8,
        dift_rec_case_nb(OPT_MEM, OPT_MEM, MO_8, EFFECT_ONE_TO_ONE | EFFECT_ASSIGN) );
}

TEST( DiftUnitTest, MEM_TAINTING ) {

    printf( "Check inital taint status of memory address 0x0000000000000000 clean ... " );
    ASSERT_EQ( 0x0, get_mem_dirty(dc, 0x0000000000000000) );
    printf( "Done\n" );

    printf( "Set addr tainted with tag 0x1 ... " );
    set_mem_dirty( dc, 0x0000000000000000, 0x1, 0 );
    ASSERT_EQ( 0x1, get_mem_dirty(dc, 0x0000000000000000) );
    printf( "Done\n" );

    printf( "Append addr with taint tag 0x2, giving 0x3 tag ... " );
    set_mem_dirty( dc, 0x0000000000000000, 0x2, 1 );
    ASSERT_EQ( 0x3, get_mem_dirty(dc, 0x0000000000000000) );
    printf( "Done\n" );

    printf( "And addr with taint tag 0x1, giving 0x1 tag ... " );
    and_mem_dirty( dc, 0x0000000000000000, 0x1 );
    ASSERT_EQ( 0x1, get_mem_dirty(dc, 0x0000000000000000) );
    printf( "Done\n" );
}

TEST( DiftUnitTest, HD_TAINTING ) {
    
    printf( "Check inital taint status of disk address 0x0000000000000000 clean ... " );
    ASSERT_EQ( 0x0, get_hd_dirty(dc, 0x0000000000000000) );
    printf( "Done\n" );

    printf( "Set addr tainted with tag 0x4 ... " );
    set_hd_dirty_or( dc, 0x0000000000000000, 0x4 );
    ASSERT_EQ( 0x4, get_hd_dirty(dc, 0x0000000000000000) );
    printf( "Done\n" );

    printf( "Append addr with taint tag 0x8, giving 0xC tag ... " );
    set_hd_dirty_or( dc, 0x0000000000000000, 0x8 );
    ASSERT_EQ( 0xc, get_hd_dirty(dc, 0x0000000000000000) );
    printf( "Done\n" );

    printf( "And addr with taint tag 0x4, giving 0x4 tag ... " );
    set_hd_dirty_and( dc, 0x0000000000000000, 0x4 );
    ASSERT_EQ( 0x4, get_hd_dirty(dc, 0x0000000000000000) );
    printf( "Done\n" );
}

TEST( DiftUnitTest, HD_MEM_TAINTING ) {

    printf( "Taint-copy 1 byte from MEM:0000000000000000(tag = 0x1) to HD:0x0000000000001000 ... " );
    copy_contamination_hd_mem( dc, 0x0000000000001000, 0x0000000000000000, 1 );
    ASSERT_EQ( 0x1, get_hd_dirty(dc, 0x0000000000001000) );
    printf( "Done\n" );
}

TEST( DiftUnitTest, MEM_HD_TAINTING ) {
 
    printf( "Taint-copy 1 byte from HD:0000000000000000(tag = 0x4) to MEM:0x0000000000001000 ... " );
    copy_contamination_mem_hd( dc, 0x0000000000001000, 0x0000000000000000, 1 );
    ASSERT_EQ( 0x4, get_mem_dirty(dc, 0x0000000000001000) );
    printf( "Done\n" );
}

