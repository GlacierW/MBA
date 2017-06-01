/*z
 *  De-coupled Information Flow Tracking (DIFT) test cases
 *
 *  Copyright (c) 2016 Chiawei Wang
 *                2016 Hao Li
 *                2016 Chuanhua, Cheng
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

#include <gmock/gmock.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <set>
#include <inttypes.h>

struct dift_context;

struct hook_functions {

    virtual ~hook_functions() {};

    virtual void exit( int ) = 0;
    virtual size_t getpagesize() = 0;
    virtual void* calloc( size_t, size_t ) = 0;
    virtual int dift_log( char const * ) = 0;
    virtual void next_block( dift_context* ) = 0;
    virtual uint8_t* alloc_hd_dirty_page() = 0;
    virtual void kick_enqptr() = 0;
    virtual void wait_dift_analysis() = 0;
    virtual void dift_rec_enqueue( uint64_t ) = 0;
    virtual void gen_rt_finish_curr_block() = 0;
    virtual void gen_rt_get_next_enqptr() = 0;
    virtual void gen_rt_enqueue_one_rec() = 0;
    virtual void gen_rt_enqueue_raddr() = 0;
    virtual void gen_rt_enqueue_waddr() = 0;
    virtual int mprotect( void*, size_t, int ) = 0;
    virtual int fflush(FILE *) = 0;
    virtual int vfprintf(FILE *, const char *, va_list) = 0;
    virtual FILE* fopen(const char *, const char *) = 0;
    virtual int is_valid_tag(const uint8_t) = 0;
    virtual int is_valid_mem_range(uint64_t, uint64_t) = 0;
    virtual int is_valid_disk_range(uint64_t, uint64_t) = 0;
    virtual void pre_generate_routine() = 0;
    virtual void init_queue() = 0;
    virtual void init_case_mapping() = 0;
    virtual void init_dift_context(dift_context*) = 0;
    virtual int pthread_create(pthread_t*, const pthread_attr_t*, void*(*)(void*), void* ) = 0;
};

struct mock_functions : hook_functions {
    MOCK_METHOD1( exit, void(int) );
    MOCK_METHOD0( getpagesize, size_t() );
    MOCK_METHOD2( calloc, void*(size_t, size_t) );
    MOCK_METHOD1( dift_log, int(char const *) );
    MOCK_METHOD1( next_block, void(dift_context*) );
    MOCK_METHOD0( alloc_hd_dirty_page, uint8_t*() );
    MOCK_METHOD0( kick_enqptr, void() );
    MOCK_METHOD0( wait_dift_analysis, void() );
    MOCK_METHOD1( dift_rec_enqueue, void(uint64_t) );
    MOCK_METHOD0( gen_rt_finish_curr_block, void() );
    MOCK_METHOD0( gen_rt_get_next_enqptr, void() );
    MOCK_METHOD0( gen_rt_enqueue_one_rec, void() );
    MOCK_METHOD0( gen_rt_enqueue_raddr, void() );
    MOCK_METHOD0( gen_rt_enqueue_waddr, void() );
    MOCK_METHOD3( mprotect, int(void*, size_t, int) );
    MOCK_METHOD1(fflush, int(FILE *));
    MOCK_METHOD3(vfprintf, int(FILE *, const char *, va_list));
    MOCK_METHOD2(fopen, FILE*(const char *, const char * ));
    MOCK_METHOD1(is_valid_tag, int(const uint8_t));
    MOCK_METHOD2(is_valid_mem_range, int(uint64_t, uint64_t));
    MOCK_METHOD2(is_valid_disk_range, int(uint64_t, uint64_t));
    MOCK_METHOD0(pre_generate_routine, void());
    MOCK_METHOD0(init_queue, void());
    MOCK_METHOD0(init_case_mapping, void());
    MOCK_METHOD1(init_dift_context, void(dift_context*));
    MOCK_METHOD4(pthread_create, int(pthread_t*, const pthread_attr_t*, void*(*)(void*), void*));
} *mock_ptr;

extern "C" {
#include "../dift.c"
}
#undef exit
#undef getpagesize
#undef calloc
#undef dift_log
#undef next_block
#undef alloc_hd_dirty_page
#undef kick_enqptr
#undef wait_dift_analysis
#undef dift_rec_enqueue
#undef gen_rt_finish_curr_block
#undef gen_rt_get_next_enqptr
#undef gen_rt_enqueue_one_rec
#undef gen_rt_enqueue_raddr
#undef gen_rt_enqueue_waddr
#undef mprotect
#undef fflush
#undef vfprintf
#undef fopen
#undef is_valid_tag
#undef is_valid_mem_range
#undef is_valid_disk_range
#undef pre_generate_routine
#undef init_queue
#undef init_case_mapping
#undef init_dift_context
#undef pthread_create

#define GEN_MOCK_OBJECT( x )  mock_functions x; mock_ptr = &x;
#define GEN_NICEMOCK_OBJECT( x )  NiceMock<mock_functions> x; mock_ptr = &x;

#define SHARE_VALUE_NOT_ZERO 0x1

using namespace ::testing;
class FixtureDiftContextInitialized : public Test {
protected:
    virtual void SetUp() {
        GEN_MOCK_OBJECT( mock );

        EXPECT_CALL( mock, calloc(_,_) ).WillRepeatedly( Invoke(calloc) );
        EXPECT_CALL( mock, getpagesize() ).WillOnce( Return(0x1000) );
        phys_ram_size = 0x10000;

        _init_dift_context( dc );
    }
    virtual void TearDown() {
        free( dc->mem_dirty_tbl );
        //XXX:dc->hd_l1_dirty_tbl entry pointers not freed for speed purpose
        free( dc->hd_l1_dirty_tbl );
    }
};

TEST( DiftContextInitialization, NORMAL ) {
    GEN_MOCK_OBJECT( mock );
    phys_ram_size = 0x10000;
    size_t const pagesize = getpagesize();

    EXPECT_CALL( mock, calloc(pagesize + phys_ram_size, _) )
    .Times( 1 ).WillOnce( Invoke(calloc) );
    EXPECT_CALL( mock, calloc(1<<HD_L1_INDEX_BITS, _) )
    .Times( 1 ).WillOnce( Invoke(calloc) );
    EXPECT_CALL( mock, getpagesize() ).Times( 1 ).WillOnce( Return(pagesize) );

    _init_dift_context( dc );

    size_t count_checked_bytes = 0;
    #define ASSERT_EQ_ACCU_SIZE( expect, real )\
        do {\
            ASSERT_EQ( expect, real );\
            count_checked_bytes += sizeof( real );\
        } while ( false )
    ASSERT_EQ_ACCU_SIZE( NULL, dc->deqptr );
    ASSERT_EQ_ACCU_SIZE( 0, dc->tail );
    ASSERT_EQ_ACCU_SIZE( Q_CHUNKS_SIZE - 1, dc->prev_tail );
    for ( int i = 0; i < 24; ++i )
        for ( int j = 0; j < 8; ++j )
            ASSERT_EQ_ACCU_SIZE( 0, dc->reg_dirty_tbl[i][j] );

    for ( int i = 0; i < 16; ++i )
        for ( int j = 0; j < 16; ++j )
            ASSERT_EQ_ACCU_SIZE( 0, dc->xmm_dirty_tbl[i][j] );

    for ( int i = 0; i < (int)phys_ram_size; ++i ) 
        ASSERT_EQ( 0, dc->mem_dirty_tbl[i] );

    count_checked_bytes += sizeof( dc->mem_dirty_tbl );

    for ( int i = 0; i < HD_L1_INDEX_BITS; ++i )
        ASSERT_EQ( NULL, dc->hd_l1_dirty_tbl[i] );

    count_checked_bytes += sizeof( dc->hd_l1_dirty_tbl );

    ASSERT_EQ( sizeof(dift_context), count_checked_bytes );
    #undef ASSERT_EQ_ACCU_SIZE
}

TEST( DiftContextInitialization, CALLOC_FAIL ) {
    GEN_NICEMOCK_OBJECT( mock );
    InSequence dummy;

    phys_ram_size = 0x1000;

    int exception = 1;

    EXPECT_CALL( mock, exit(Ne(1)) ).Times(0 );
    EXPECT_CALL( mock, calloc(_, _) )
        .WillOnce( Invoke(calloc) ).WillOnce( ReturnNull() );
    EXPECT_CALL( mock, dift_log(_) );
    EXPECT_CALL( mock, exit(1) ).WillOnce( Throw(exception) );

    EXPECT_CALL( mock, calloc(_, _) ).WillOnce( ReturnNull() );
    EXPECT_CALL( mock, dift_log(_) );
    EXPECT_CALL( mock, exit(1) ).WillOnce( Throw(exception) );

    ASSERT_ANY_THROW( {
        _init_dift_context( dc );
    } );
    ASSERT_ANY_THROW( {
        _init_dift_context( dc );
    } );
}

TEST( RecordCase, DUMP_CASE ) {
#define dump_case_nb( name ) \
    fprintf( stderr, "%-27s: %08x\n", #name, case_list[name] ), dumped_case_count++
    int dumped_case_count = 0;
    dump_case_nb( INSIDE_REG_ASSIGN );
    dump_case_nb( INSIDE_REG_APPEND );
    dump_case_nb( REG_REG_OO_ASSIGN_MO8 );
    dump_case_nb( REG_REG_OO_ASSIGN_MO16 );
    dump_case_nb( REG_REG_OO_ASSIGN_MO32 );
    dump_case_nb( REG_REG_OO_ASSIGN_MO64 );
    dump_case_nb( REG_REG_OO_APPEND_MO8 );
    dump_case_nb( REG_REG_OO_APPEND_MO16 );
    dump_case_nb( REG_REG_OO_APPEND_MO32 );
    dump_case_nb( REG_REG_OO_APPEND_MO64 );
    dump_case_nb( REG_REG_MIX_APPEND_MO8 );
    dump_case_nb( REG_REG_MIX_APPEND_MO16 );
    dump_case_nb( REG_REG_MIX_APPEND_MO32 );
    dump_case_nb( REG_REG_MIX_APPEND_MO64 );
    dump_case_nb( REG_MEM_OO_ASSIGN_MO8 );
    dump_case_nb( REG_MEM_OO_ASSIGN_MO16 );
    dump_case_nb( REG_MEM_OO_ASSIGN_MO32 );
    dump_case_nb( REG_MEM_OO_ASSIGN_MO64 );
    dump_case_nb( REG_MEM_OO_APPEND_MO8 );
    dump_case_nb( REG_MEM_OO_APPEND_MO16 );
    dump_case_nb( REG_MEM_OO_APPEND_MO32 );
    dump_case_nb( REG_MEM_OO_APPEND_MO64 );
    dump_case_nb( REG_MEM_MIX_ASSIGN_MO16 );
    dump_case_nb( REG_MEM_MIX_ASSIGN_MO32 );
    dump_case_nb( REG_MEM_MIX_ASSIGN_MO64 );
    dump_case_nb( REG_MEM_MIX_APPEND_MO8 );
    dump_case_nb( REG_MEM_MIX_APPEND_MO16 );
    dump_case_nb( REG_MEM_MIX_APPEND_MO32 );
    dump_case_nb( REG_MEM_MIX_APPEND_MO64 );
    dump_case_nb( MEM_REG_OO_ASSIGN_MO8 );
    dump_case_nb( MEM_REG_OO_ASSIGN_MO16 );
    dump_case_nb( MEM_REG_OO_ASSIGN_MO32 );
    dump_case_nb( MEM_REG_OO_ASSIGN_MO64 );
    dump_case_nb( MEM_REG_OO_APPEND_MO8 );
    dump_case_nb( MEM_REG_OO_APPEND_MO16 );
    dump_case_nb( MEM_REG_OO_APPEND_MO32 );
    dump_case_nb( MEM_REG_OO_APPEND_MO64 );
    dump_case_nb( MEM_REG_MIX_ASSIGN_MO16 );
    dump_case_nb( MEM_REG_MIX_ASSIGN_MO32 );
    dump_case_nb( MEM_REG_MIX_ASSIGN_MO64 );
    dump_case_nb( MEM_REG_MIX_APPEND_MO8 );
    dump_case_nb( MEM_REG_MIX_APPEND_MO16 );
    dump_case_nb( MEM_REG_MIX_APPEND_MO32 );
    dump_case_nb( MEM_REG_MIX_APPEND_MO64 );
    dump_case_nb( MEM_MEM_OO_ASSIGN_MO8 );
    dump_case_nb( MEM_MEM_OO_ASSIGN_MO16 );
    dump_case_nb( MEM_MEM_OO_ASSIGN_MO32 );
    dump_case_nb( MEM_MEM_OO_ASSIGN_MO64 );
    dump_case_nb( REG_IM_CLEAR_MO8 );
    dump_case_nb( REG_IM_CLEAR_MO16 );
    dump_case_nb( REG_IM_CLEAR_MO32 );
    dump_case_nb( REG_IM_CLEAR_MO64 );
    dump_case_nb( MEM_IM_CLEAR_MO8 );
    dump_case_nb( MEM_IM_CLEAR_MO16 );
    dump_case_nb( MEM_IM_CLEAR_MO32 );
    dump_case_nb( MEM_IM_CLEAR_MO64 );
    dump_case_nb( XMM_XMM_OO_ASSIGN_MO32 );
    dump_case_nb( XMM_XMM_OO_ASSIGN_MO64 );
    dump_case_nb( XMM_XMM_OO_ASSIGN_MO64LH );
    dump_case_nb( XMM_XMM_OO_ASSIGN_MO64HL );
    dump_case_nb( XMM_XMM_OO_ASSIGN_MO128 );
    dump_case_nb( XMM_REG_OO_ASSIGN_MO32 );
    dump_case_nb( XMM_REG_OO_ASSIGN_MO64 );
    dump_case_nb( REG_XMM_OO_ASSIGN_MO32 );
    dump_case_nb( REG_XMM_OO_ASSIGN_MO64 );
    dump_case_nb( XMM_MEM_OO_ASSIGN_MO32 );
    dump_case_nb( XMM_MEM_OO_ASSIGN_MO64 );
    dump_case_nb( XMM_MEM_OO_ASSIGN_MO64H );
    dump_case_nb( XMM_MEM_OO_ASSIGN_MO128 );
    dump_case_nb( MEM_XMM_OO_ASSIGN_MO32 );
    dump_case_nb( MEM_XMM_OO_ASSIGN_MO64 );
    dump_case_nb( MEM_XMM_OO_ASSIGN_MO64H );
    dump_case_nb( MEM_XMM_OO_ASSIGN_MO128 );
    dump_case_nb( XMM_XMM_OO_ASSIGN_DUP_MO32L );
    dump_case_nb( XMM_XMM_OO_ASSIGN_DUP_MO32H );
    dump_case_nb( XMM_XMM_OO_ASSIGN_DUP_MO64 );
    dump_case_nb( XMM_MEM_OO_ASSIGN_DUP_MO32L );
    dump_case_nb( XMM_MEM_OO_ASSIGN_DUP_MO32H );
    dump_case_nb( XMM_MEM_OO_ASSIGN_DUP_MO64 );
    dump_case_nb( XMM_IM_CLEAR );

    int const case_count_in_case_list = sizeof( case_list )/sizeof(*case_list );
    ASSERT_EQ( dumped_case_count, case_count_in_case_list );
}

#define COUNT_ELEMENT(x) sizeof(x)/sizeof(*x)
TEST( RecordCase, PATTERN_CHECK ) {

    int const case_count = COUNT_ELEMENT( case_list );
    std::set<int> case_set;
    for ( size_t i = 0; i < case_count; i++ ) {
        EXPECT_EQ( 0xc000, case_list[i]&0xc000 ) << "With i: " << i;
        case_set.insert( case_list[i] );
    }

    ASSERT_EQ( case_count, case_set.size() );
}

TEST( RecordCase, INIT_CASE_MAPPING ) {
    _init_case_mapping();

    ASSERT_EQ( INSIDE_REG_ASSIGN, case_mapping[((0xc0 | OPT_REG << 4 | OPT_REG << 2 | MO_8) << 8) | EFFECT_INSIDE_REG | EFFECT_ASSIGN] );
    ASSERT_EQ( REG_REG_MIX_APPEND_MO16, case_mapping[((0xc0 | OPT_REG << 4 | OPT_REG << 2 | MO_16) << 8) | EFFECT_MIX | EFFECT_APPEND] );
    ASSERT_EQ( REG_MEM_OO_ASSIGN_MO32, case_mapping[((0xc0 | OPT_MEM << 4 | OPT_REG << 2 | MO_32) << 8) | EFFECT_ONE_TO_ONE | EFFECT_ASSIGN] );
    ASSERT_EQ( MEM_REG_OO_APPEND_MO64, case_mapping[((0xc0 | OPT_REG << 4 | OPT_MEM << 2 | MO_64) << 8) | EFFECT_ONE_TO_ONE | EFFECT_APPEND] );

    std::set<int> mapped_case_set;
    for ( size_t i = 0; i < COUNT_ELEMENT(case_mapping); i++ ) {
        uint8_t mapped_case = case_mapping[i];
        if ( mapped_case == 0xff )
            continue;

        EXPECT_EQ( mapped_case, case_mapping[case_list[mapped_case]] );
        mapped_case_set.insert( mapped_case );
    }
    int const case_count = COUNT_ELEMENT( case_list );
    ASSERT_EQ( case_count, mapped_case_set.size() );
    ASSERT_LT( case_count, 0xff );
}

TEST( RecordCase, GET_CASE_NB ) {
    GEN_NICEMOCK_OBJECT( mock );

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

    EXPECT_CALL( mock, dift_log(_) ).Times( 0 );
    int const case_count = COUNT_ELEMENT( case_list );
    std::set<int> nb_set;
    for ( int i = 0; i < case_count; i++ ) {
        uint16_t const rec_case = case_list[i];
        uint8_t  const src_type = (rec_case>>12) &  0x3;
        uint8_t  const dst_type = (rec_case>>10) &  0x3;
        uint8_t  const ot       = (rec_case>> 8) &  0x3;
        uint8_t  const effect   =  rec_case      & 0xff;
        
        nb_set.insert( dift_rec_case_nb(dst_type, src_type, ot, effect) );
    }

    ASSERT_EQ( case_count, nb_set.size() );

    EXPECT_CALL( mock, dift_log(_) ).Times( 2 );
    EXPECT_CALL( mock, exit(1) );

    dift_rec_case_nb( 0, 0, 0, 0 );
}

class RecordQueue : public FixtureDiftContextInitialized{};
TEST_F( RecordQueue, INIT_QUEUE ) {
    _init_queue();

    ASSERT_EQ( 1, head );
    ASSERT_EQ( 0, prev_head );
    ASSERT_EQ( CHUNK_FILLING, q_chunks_flag[prev_head] ); 

    for ( int i = 1; i < Q_CHUNKS_SIZE; ++i ) {
        EXPECT_EQ( CHUNK_AVAILABLE, q_chunks_flag[i] );
        EXPECT_EQ( &q_records[(i * SIZE_OF_CHUNK)/sizeof(uint64_t)], q_chunks_ptr[i] );
    }

    ASSERT_EQ( q_records, enqptr );
}

TEST_F( RecordQueue, ENQUEUE ) {
    GEN_MOCK_OBJECT( mock );

    EXPECT_CALL( mock, calloc(_,_) )
    .WillRepeatedly( Invoke(calloc) );
    EXPECT_CALL( mock, getpagesize() ).WillRepeatedly( Return(getpagesize()) );

    _init_dift_context( dc );
    uint64_t const rec = 0x1234567812345678;

    EXPECT_EQ( 0, prev_head );
    EXPECT_EQ( 1, head );

    // single enqueue
    ASSERT_EQ( q_chunks_ptr[prev_head], enqptr );
    ASSERT_EQ( CHUNK_FILLING, q_chunks_flag[prev_head] );

    _dift_rec_enqueue( rec );
    
    ASSERT_EQ( q_chunks_ptr[prev_head] + 1, enqptr );
    ASSERT_EQ( CHUNK_FILLING, q_chunks_flag[prev_head] );

    // cross 2 chunks enqueue
    int const enq_chunk      = 2;
    int const enq_nb         = ( (SIZE_OF_CHUNK) / sizeof(uint64_t) ) * enq_chunk;
    int const prev_head_before_enq   = prev_head;
    int const head_before_enq        = head;

    for ( int i = 0; i < enq_nb; ++i )
        _dift_rec_enqueue( rec );

    int const prev_head_after_enq    = prev_head;
    int const head_after_enq         = head;
    ASSERT_EQ( prev_head_before_enq + enq_chunk, prev_head_after_enq )
        << "With enq_chunk:" << enq_chunk;
    ASSERT_EQ( head_before_enq + enq_chunk, head_after_enq )
        << "With enq_chunk:" << enq_chunk;
    
    ASSERT_EQ( (prev_head_before_enq + 2) % Q_CHUNKS_SIZE, prev_head );
    ASSERT_EQ( (prev_head + 1) % Q_CHUNKS_SIZE, head );
    ASSERT_EQ( CHUNK_FILLED, q_chunks_flag[prev_head_before_enq] );
    ASSERT_EQ( q_chunks_ptr[prev_head] + 1, enqptr );
    ASSERT_EQ( CHUNK_FILLING, q_chunks_flag[prev_head] );
}

TEST_F( RecordQueue, DEQUEUE ) {
    //XXX: has dependency to the test DIFT_RECORD_ENQUEUE
    GEN_MOCK_OBJECT( mock );

    EXPECT_CALL( mock, next_block(_) ).WillOnce( Invoke(_next_block) );
    int const deq_nb = ( SIZE_OF_CHUNK ) / sizeof( uint64_t );

    // first dequeue
    uint64_t rec = rec_dequeue( dc );

    EXPECT_CALL( mock, next_block(_) ).WillOnce( Invoke(_next_block) );
    EXPECT_EQ( 0, dc->prev_tail );
    EXPECT_EQ( 1, dc->tail );
    ASSERT_EQ( 0x1234567812345678, rec );
    ASSERT_EQ( CHUNK_CONSUMING, q_chunks_flag[dc->prev_tail] );

    for ( int i = deq_nb - 1; i >= 0; --i )
        rec = rec_dequeue( dc );
}

TEST_F( RecordQueue, NEXT_BLOCK) {
    uint32_t const old_prev_tail = dc->prev_tail;
    uint32_t const old_tail      = dc->tail;

    q_chunks_flag[dc->tail] = CHUNK_FILLED;
    _next_block( dc );

    ASSERT_EQ( CHUNK_AVAILABLE, q_chunks_flag[ old_prev_tail ] ) << "expected: CHUNK_AVAILABLE";
    ASSERT_EQ( CHUNK_CONSUMING, q_chunks_flag[ dc->prev_tail ] ) << "expected: CHUNK_CONSUMING";
    ASSERT_EQ( q_chunks_ptr[ old_tail ], dc->deqptr );
    ASSERT_EQ( old_tail, dc->prev_tail );
    ASSERT_EQ( (old_tail + 1) % Q_CHUNKS_SIZE, dc->tail );
}

TEST_F( RecordQueue, KICK_ENQPTR) {
    uint64_t const old_prev_head = prev_head;
    uint64_t const old_head = head;

    _kick_enqptr();

    ASSERT_EQ( CHUNK_FILLED, q_chunks_flag[old_prev_head] ) << "expected: CHUNK_FILLED";

    ASSERT_EQ( CHUNK_FILLING, q_chunks_flag[prev_head] ) << "expected: CHUNK_FILLING";
    ASSERT_EQ( (uint64_t*)q_chunks_ptr[old_head], enqptr );
    ASSERT_EQ( old_head, prev_head );
    ASSERT_EQ( (old_head + 1) % Q_CHUNKS_SIZE, head );

    // should not do anything
    _kick_enqptr();

    ASSERT_EQ( CHUNK_FILLED, q_chunks_flag[old_prev_head] ) << "expected: CHUNK_FILLED";

    ASSERT_EQ( CHUNK_FILLING, q_chunks_flag[prev_head] ) << "expected: CHUNK_FILLING";
    ASSERT_EQ( (uint64_t*)q_chunks_ptr[old_head], enqptr );
    ASSERT_EQ( old_head, prev_head );
    ASSERT_EQ( (old_head + 1) % Q_CHUNKS_SIZE, head );
}

class Tainting :public FixtureDiftContextInitialized{};
TEST_F( Tainting, SET_MEM_DIRTY ) {
    GEN_MOCK_OBJECT( mock );
    uint64_t const test_addr = 0x0000000000000000;
    EXPECT_CALL( mock, is_valid_mem_range(_,_) ).WillRepeatedly( Invoke(_is_valid_mem_range) );
    EXPECT_CALL( mock, is_valid_tag(_) ).WillRepeatedly( Invoke(_is_valid_tag) );

    ASSERT_EQ( 0x0, get_mem_dirty(dc, test_addr) )
        << "should be clear before taint test";

    ASSERT_EQ( DIFT_SUCCESS, set_mem_dirty(dc, test_addr, 1, 0x1, false) );
    ASSERT_EQ( 0x1, get_mem_dirty(dc, test_addr) );

    ASSERT_EQ( DIFT_SUCCESS, set_mem_dirty(dc, test_addr, 1, 0x2, true) );
    ASSERT_EQ( 0x3, get_mem_dirty(dc, test_addr) );

    ASSERT_EQ( DIFT_SUCCESS, and_mem_dirty(dc, test_addr, 1, 0x1) );
    ASSERT_EQ( 0x1, get_mem_dirty(dc, test_addr) );
}

TEST_F( Tainting, GET_MEM_DIRTY ) {
    GEN_MOCK_OBJECT( mock );
    uint64_t const test_addr = 0x0;
    EXPECT_CALL( mock, is_valid_mem_range(_,_) ).WillRepeatedly( Invoke(_is_valid_mem_range) );
    EXPECT_CALL( mock, is_valid_tag(_) ).WillRepeatedly( Invoke(_is_valid_tag) );

    ASSERT_EQ( 0x0, get_mem_dirty(dc, test_addr) );

    set_mem_dirty( dc, test_addr, 1, 0x4, false );
    ASSERT_EQ( 0x4, get_mem_dirty(dc, test_addr) );
    /*TODO:check below two calls fail
    get_mem_dirty( dc, -1 );
    get_mem_dirty( dc, phys_ram_size );
    */
}

TEST_F( Tainting, SET_HD_DIRTY ) {
    GEN_MOCK_OBJECT( mock );

    EXPECT_CALL( mock, is_valid_disk_range(_,_) ).WillRepeatedly( Invoke(_is_valid_disk_range) );
    EXPECT_CALL( mock, is_valid_tag(_) ).WillRepeatedly( Invoke(_is_valid_tag) );
    CONTAMINATION_RECORD page[HD_L2_SIZE] = {};
    EXPECT_CALL( mock, alloc_hd_dirty_page() ).WillOnce(Return(page) );
    uint64_t const test_addr = 0x0000000000000000;

    ASSERT_EQ( 0x0, get_hd_dirty(dc, test_addr) )
        << "should be clear before taint test";

    set_hd_dirty_or( dc, test_addr, 1, 0x4 );
    ASSERT_EQ( 0x4, get_hd_dirty(dc, test_addr) );

    set_hd_dirty_or( dc, test_addr, 1, 0x8 );
    ASSERT_EQ( 0xc, get_hd_dirty(dc, test_addr) );

    set_hd_dirty_and( dc, test_addr, 1, 0x4 );
    ASSERT_EQ( 0x4, get_hd_dirty(dc, test_addr) );
}

TEST_F( Tainting, GET_HD_DIRTY ) {
    GEN_MOCK_OBJECT( mock );
    uint64_t const test_addr = 0x0000000000000000;
    EXPECT_CALL( mock, is_valid_disk_range(_,_) ).WillRepeatedly( Return(true) );
    EXPECT_CALL( mock, is_valid_tag(_) ).WillRepeatedly( Return(true) );

    ASSERT_EQ( 0x0, get_hd_dirty(dc, test_addr) );

    CONTAMINATION_RECORD page[HD_L2_SIZE] = {};
    EXPECT_CALL( mock, alloc_hd_dirty_page() ).WillOnce(Return(page) );

    set_hd_dirty_or( dc, test_addr, 1, 0x4 );
    ASSERT_EQ( 0x4, get_hd_dirty(dc, test_addr) );
    /*TODO:check above two calls fail
    get_hd_dirty( dc, -1 );
    get_hd_dirty( dc, (1<<HD_MAX_SIZE) );
    */
}

TEST_F( Tainting, MEM_TO_HD ) {
    GEN_MOCK_OBJECT( mock );
    EXPECT_CALL( mock, is_valid_mem_range(_,_) ).WillRepeatedly( Invoke(_is_valid_mem_range) );

    uint64_t const tag = SHARE_VALUE_NOT_ZERO;
    uint64_t const hd_addr        = 0x0000000000000000;
    uint64_t const mem_addr       = 0x0000000000000000;

    ASSERT_EQ( NULL, dc->hd_l1_dirty_tbl[HD_L1_INDEX(hd_addr)] )
        << "check HD clear before test";

    EXPECT_CALL( mock, is_valid_disk_range(_,_) ).WillRepeatedly( Invoke(_is_valid_disk_range) );
    EXPECT_CALL( mock, is_valid_tag(_) ).WillRepeatedly( Invoke(_is_valid_tag) );
    CONTAMINATION_RECORD page[ HD_L2_SIZE ] = {};
    EXPECT_CALL( mock, alloc_hd_dirty_page() ).WillOnce( Return(page) );

    set_mem_dirty( dc, mem_addr, 1, tag, false );
    copy_contamination_hd_mem( dc, hd_addr, mem_addr, 1 );
    ASSERT_EQ( tag, get_hd_dirty(dc, hd_addr) );

    uint64_t const copy_len       = phys_ram_size;
    uint64_t const begin_hd_addr  = 0x0000000000000000;
    uint64_t const last_hd_addr   = begin_hd_addr + copy_len - 1;
    uint64_t const begin_mem_addr = 0x0000000000000000;
    uint64_t const last_mem_addr  = begin_mem_addr + copy_len - 1;

    ASSERT_EQ( NULL, dc->hd_l1_dirty_tbl[HD_L1_INDEX(last_hd_addr)] )
        << "check HD clear before test";

    EXPECT_CALL( mock, alloc_hd_dirty_page() ).WillRepeatedly( Return(page) );

    set_mem_dirty( dc, last_mem_addr, 1, tag, false);
    copy_contamination_hd_mem( dc, begin_hd_addr, begin_mem_addr, copy_len );
    ASSERT_EQ( tag, get_hd_dirty(dc, last_hd_addr) );
    int const count_pages = 1 + ( (copy_len - 1) / HD_L2_SIZE );
    for (int i = 0; i < count_pages; i++) {
        uint64_t addr = begin_hd_addr + i * HD_L2_SIZE;
        ASSERT_TRUE( dc->hd_l1_dirty_tbl[HD_L1_INDEX(addr)]!=NULL )
            << "copy destination should be allocated";
    }
}

TEST_F( Tainting, HD_TO_MEM ) {
    GEN_MOCK_OBJECT( mock );
    EXPECT_CALL( mock, is_valid_mem_range(_,_) ).WillRepeatedly( Invoke(_is_valid_mem_range) );

    uint64_t const tag = SHARE_VALUE_NOT_ZERO;
    uint64_t const mem_addr = 0x0000000000000000;
    uint64_t const hd_addr  = 0x0000000000000000;

    ASSERT_EQ( 0x0, get_mem_dirty(dc, mem_addr) )
        << "check MEM clear before test";

    EXPECT_CALL( mock, is_valid_disk_range(_,_) ).WillRepeatedly( Invoke(_is_valid_disk_range) );
    EXPECT_CALL( mock, is_valid_tag(_) ).WillRepeatedly( Invoke(_is_valid_tag) );
    CONTAMINATION_RECORD page[HD_L2_SIZE] = {};

    EXPECT_CALL( mock, alloc_hd_dirty_page() ).WillRepeatedly( Return(page) );
    set_hd_dirty_or( dc, hd_addr, 1, tag );
    copy_contamination_mem_hd( dc, mem_addr, hd_addr, 1 );
    ASSERT_EQ( tag, get_mem_dirty(dc, mem_addr) );

    //TODO:test full disk copy (fail?)
    //     test exact mem size copy
    uint64_t const copy_len       = phys_ram_size;
    uint64_t const begin_hd_addr  = 0x0000000000000000;
    uint64_t const last_hd_addr   = begin_hd_addr + copy_len - 1;
    uint64_t const begin_mem_addr = 0x0000000000000000;
    uint64_t const last_mem_addr  = begin_mem_addr + copy_len - 1;

    ASSERT_EQ( 0x0, get_mem_dirty(dc, last_mem_addr) )
        << "check init clear";

    EXPECT_CALL( mock, alloc_hd_dirty_page() ).WillRepeatedly( Return(page) );

    set_hd_dirty_or( dc, last_hd_addr, 1, tag );
    copy_contamination_mem_hd( dc, begin_mem_addr, begin_hd_addr, copy_len );
    ASSERT_EQ( tag, get_hd_dirty(dc, last_mem_addr) );
}

TEST_F (Tainting, INVALID_RANGE ) {
    GEN_MOCK_OBJECT( mock );

    uint64_t const mem_addr = SHARE_VALUE_NOT_ZERO;
    uint64_t const hd_addr  = SHARE_VALUE_NOT_ZERO;

    ON_CALL( mock, is_valid_mem_range(_,_) ).WillByDefault( Return(1) );
    ON_CALL( mock, is_valid_disk_range(_,_) ).WillByDefault( Return(0) );
    copy_contamination_mem_hd( dc, mem_addr, hd_addr, 1 );

    ON_CALL( mock, is_valid_mem_range(_,_) ).WillByDefault( Return(0) );
    ON_CALL( mock, is_valid_disk_range(_,_) ).WillByDefault( Return(1) );
    copy_contamination_mem_hd( dc, mem_addr, hd_addr, 1 );
}

TEST( Others, DIFT_SYNC) {
    GEN_MOCK_OBJECT( mock );
    InSequence dummy;

    dift_code_loc = SHARE_VALUE_NOT_ZERO;
    dift_code_cntr = SHARE_VALUE_NOT_ZERO;

    dift_thread_ok_signal = 1;

    EXPECT_CALL( mock, dift_rec_enqueue(REC_END_SYMBOL | REC_BEFORE_BLOCK_BEGIN) );
    EXPECT_CALL( mock, dift_rec_enqueue((((uint64_t)dift_code_loc<<16)|dift_code_cntr)<<16) );
    EXPECT_CALL( mock, dift_rec_enqueue(REC_END_SYMBOL | REC_SYNC) );
    EXPECT_CALL( mock, kick_enqptr() );
    EXPECT_CALL( mock, wait_dift_analysis() ).WillOnce( Invoke(_wait_dift_analysis) );

    dift_sync();

    ASSERT_EQ( 0, dift_code_cntr );
    ASSERT_EQ( 0, dift_thread_ok_signal );
}

static void dump_func_insn( const char* func_name, uint8_t* func_ptr ) {

    int fd = -1;
    int rt_sz;

    ASSERT_NE( -1, (fd = open("rt_dump", O_RDWR | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO)) );
    ftruncate( fd, 0 );

    fprintf( stderr, "\n=============== %s ==============\n", func_name );
    fflush( stdout );

    for( rt_sz = 512 - 1; rt_sz >=0 && func_ptr[rt_sz] != 0xc3; --rt_sz );
    write( fd, func_ptr, rt_sz + 1 ); // rt_sz as index, thereby + 1
    system( "ndisasm -b 64 rt_dump >&2" );

    fprintf( stderr, "=================================\n" );
    fflush( stdout );

    unlink( "rt_dump" );
    ASSERT_LT( func_ptr+rt_sz, pre_generated_routine+sizeof(pre_generated_routine)-1 );
}

ACTION_P(ExpectZero, addr) { EXPECT_EQ(0, *addr); }
TEST( PregenedRoutine, CONTENT ) {
    GEN_NICEMOCK_OBJECT( mock );

    EXPECT_CALL( mock, gen_rt_get_next_enqptr() ).WillOnce(DoAll(
                ExpectZero( rt_get_next_enqptr ), Invoke( _gen_rt_get_next_enqptr )
                ));
    EXPECT_CALL( mock, gen_rt_finish_curr_block() ).WillOnce(DoAll(
                ExpectZero( rt_finish_curr_block), Invoke(_gen_rt_finish_curr_block )
                ));
    EXPECT_CALL( mock, gen_rt_enqueue_one_rec() ).WillOnce(DoAll(
                ExpectZero( rt_enqueue_one_rec), Invoke(_gen_rt_enqueue_one_rec )
                ));
    EXPECT_CALL( mock, gen_rt_enqueue_raddr() ).WillOnce(DoAll(
                ExpectZero( rt_enqueue_raddr), Invoke(_gen_rt_enqueue_raddr )
                ));
    EXPECT_CALL( mock, gen_rt_enqueue_waddr() ).WillOnce(DoAll(
                ExpectZero( rt_enqueue_waddr), Invoke(_gen_rt_enqueue_waddr )
                ));
    EXPECT_CALL( mock, mprotect(_, _, _));
    // This test require human recognition due to the ad-hoc string format mapping
    memset( pre_generated_routine, 0, sizeof(pre_generated_routine) );
    _pre_generate_routine();

    // Ordered by function pointer
    const char* func_name[] = {
        "rt_get_next_enqptr",
        "rt_finish_curr_block",
        "rt_enqueue_one_rec",
        "rt_enqueue_raddr",
        "rt_enqueue_waddr"
    };
    uint8_t* func_ptr[] = {
        rt_get_next_enqptr,
        rt_finish_curr_block,
        rt_enqueue_one_rec,
        rt_enqueue_raddr,
        rt_enqueue_waddr
    };

    for( uint32_t i = 0; i < sizeof(func_name) / sizeof(char*); ++i )
        dump_func_insn( func_name[i], func_ptr[i] );
}

TEST( PregenedRoutine, MPROTECT_FAIL ) {
    GEN_NICEMOCK_OBJECT( mock );

    EXPECT_CALL( mock, gen_rt_get_next_enqptr() );
    EXPECT_CALL( mock, gen_rt_finish_curr_block() );
    EXPECT_CALL( mock, gen_rt_enqueue_one_rec() );
    EXPECT_CALL( mock, gen_rt_enqueue_raddr() );
    EXPECT_CALL( mock, gen_rt_enqueue_waddr() );
    EXPECT_CALL( mock, mprotect(_, _, _)).WillOnce( Return(-1) );
    EXPECT_CALL( mock, exit(1) ).WillOnce( Throw(1) );
    // This test require human recognition due to the ad-hoc string format mapping
    memset( pre_generated_routine, 0, sizeof(pre_generated_routine) );
    ASSERT_ANY_THROW( {_pre_generate_routine();} );
}

ACTION_P(ExpectArg0Divisible, n) { EXPECT_EQ(0, (uint64_t)arg0%n); }
ACTION_P(ExpectArg1Divisible, n) { EXPECT_EQ(0, (uint64_t)arg1%n); }
class ParameterizedPagesize : public TestWithParam<int> {};
TEST_P( ParameterizedPagesize, MPROTECT ) {
    GEN_MOCK_OBJECT( mock );

    size_t const pagesize = 4096<<GetParam();
    EXPECT_CALL( mock, gen_rt_get_next_enqptr() );
    EXPECT_CALL( mock, gen_rt_finish_curr_block() );
    EXPECT_CALL( mock, gen_rt_enqueue_one_rec() );
    EXPECT_CALL( mock, gen_rt_enqueue_raddr() );
    EXPECT_CALL( mock, gen_rt_enqueue_waddr() );
    EXPECT_CALL( mock, getpagesize() ).WillOnce( Return(pagesize) );
    EXPECT_CALL( mock, mprotect(_, _, PROT_READ | PROT_WRITE | PROT_EXEC))
        .WillOnce( DoAll(
            ExpectArg0Divisible(pagesize), 
            ExpectArg1Divisible(pagesize),
            Return(0)
            ));
    // This test require human recognition due to the ad-hoc string format mapping
    _pre_generate_routine();
}
INSTANTIATE_TEST_CASE_P( dummy, ParameterizedPagesize, Range(1, 5) );

TEST( DiftUnitTest, MEM_RANGE ) {
    phys_ram_size = 0x0100;
    bool valid_mem_range = _is_valid_mem_range( 0x0, 0 );
    ASSERT_EQ( true, valid_mem_range );

    valid_mem_range = _is_valid_mem_range( 0x100, 0 );
    ASSERT_EQ( true, valid_mem_range );

    valid_mem_range = _is_valid_mem_range( 0x0, 0x101 );
    ASSERT_EQ( false, valid_mem_range );

    valid_mem_range = _is_valid_mem_range( 0x100, 0x1 );
    ASSERT_EQ( false, valid_mem_range );

    valid_mem_range = _is_valid_mem_range( 0x101, 0x1 );
    ASSERT_EQ( false, valid_mem_range );
}

TEST( DiftUnitTest, HD_RANGE ) {
    // HD_MAX_SIZE 0x0000001000000000
    bool valid_disk_range = _is_valid_disk_range( 0x0, 0x0000001000000000 ) ;
    ASSERT_EQ( true, valid_disk_range );

    valid_disk_range = _is_valid_disk_range( 0x0000001000000000, 0x0 );
    ASSERT_EQ( true, valid_disk_range );

    valid_disk_range = _is_valid_disk_range( 0x0, 0x0000001000000001 );
    ASSERT_EQ( false, valid_disk_range );

    valid_disk_range = _is_valid_disk_range( 0x0000001000000000, 0x0000000000000001 );
    ASSERT_EQ( false, valid_disk_range );

    valid_disk_range = _is_valid_disk_range( 0x0000001000000001, 0x0 );
    ASSERT_EQ( false, valid_disk_range );
}

TEST( DiftUnitTest, HD_DIRTY_PAGE ) {
    GEN_MOCK_OBJECT( mock );
    EXPECT_CALL(mock, calloc(_,_)).WillRepeatedly(Invoke(calloc));
    CONTAMINATION_RECORD* rec = _alloc_hd_dirty_page();
    int traversal, rec_size = 1 << HD_L2_INDEX_BITS;
    for ( traversal = 0 ; traversal < rec_size ; traversal++ ) {
        ASSERT_EQ( NULL, rec[traversal] ); // make sure rec is empty
        ASSERT_EQ( sizeof(rec[traversal] ), sizeof(CONTAMINATION_RECORD) ); // make sure every rec's size is correct
    } // for
}

TEST( DiftUnitTest, HD_DIRTY_PAGE_ERROR ) {
    InSequence dummy;
    GEN_MOCK_OBJECT( mock );
    EXPECT_CALL(mock, calloc(_,_)).WillOnce(ReturnNull());
    EXPECT_CALL(mock, dift_log(_)).Times(1);
    EXPECT_CALL(mock, exit(1)).Times(1);

    CONTAMINATION_RECORD* rec = _alloc_hd_dirty_page();
    ASSERT_EQ( NULL, rec ); // make sure rec is empty
}

TEST( DiftUnitTest, DIFT_ENABLE ) {
    
    // when DIFT is enabled
    dift_enabled = true;
    ASSERT_EQ( false, dift_switch_pending );
    dift_enable();
    ASSERT_EQ( false, dift_switch_pending );

    // when DIFT is disabled
    dift_enabled = false;
    ASSERT_EQ( false, dift_switch_pending );
    dift_enable();
    ASSERT_EQ( true, dift_switch_pending );

    dift_switch_pending = false;
}

TEST( DiftUnitTest, DIFT_DISABLE ) {
    
    // when DIFT is disabled
    dift_enabled = false;
    ASSERT_EQ( false, dift_switch_pending );
    dift_disable();
    ASSERT_EQ( false, dift_switch_pending );

    // when DIFT is enabled
    dift_enabled = true;
    ASSERT_EQ( false, dift_switch_pending );
    dift_disable();
    ASSERT_EQ( true, dift_switch_pending );

    dift_switch_pending = false;
}

TEST( DiftUnitTest, DIFT_IS_ENABLED ) {
    dift_enabled = true;
    ASSERT_EQ( true, dift_is_enabled() );
    dift_enabled = false;
    ASSERT_EQ( false, dift_is_enabled() );
}

TEST( DiftUnitTest, DIFT_LOG_OUTPUT ) {
    FILE *readLog;
    const int length = 10;
    int freadLen = 0;
    char log[length];
    bzero( log, length );

    GEN_MOCK_OBJECT( mock );
    EXPECT_CALL(mock, vfprintf(_,_,_)).WillOnce(Invoke(vfprintf));
    EXPECT_CALL(mock, fflush(_)).WillOnce(Invoke(fflush));

    dift_logfile = fopen( DIFT_LOG, "w+" );
    int dift_log_code = _dift_log("Unit_Test");
    fclose( dift_logfile );
    readLog = fopen( "dift.log", "r" );
    freadLen = fread( log, sizeof(char), 10, readLog );
    fclose( readLog );
    ASSERT_STREQ( log, "Unit_Test");
    ASSERT_EQ( dift_log_code, freadLen );

}

TEST( DiftUnitTest, DIFT_LOG_SEQUENCE ) {
    InSequence dummy;
    GEN_MOCK_OBJECT( mock );
    EXPECT_CALL(mock,dift_log(_)).WillRepeatedly(Invoke(_dift_log));
    EXPECT_CALL(mock,vfprintf(_,_,_)).WillRepeatedly(Invoke(vfprintf));
    EXPECT_CALL(mock,fflush(_)).Times(1);
    dift_logfile = fopen( DIFT_LOG, "w+" );
    int dift_log_code = _dift_log("");
    fclose( dift_logfile );
    ASSERT_EQ( dift_log_code, 0 );
}

TEST( DiftUnitTest, DIFT_LOG_NULL ) {
    GEN_MOCK_OBJECT( mock );
    EXPECT_CALL(mock, dift_log(_)).WillRepeatedly(Invoke(_dift_log));
    EXPECT_CALL(mock, vfprintf(_,_,_)).Times(0);
    EXPECT_CALL(mock, fflush(_)).Times(0);
    dift_logfile = NULL;
    int dift_log_code = _dift_log("");
    ASSERT_EQ( dift_log_code, -1 );
}

TEST( DiftUnitTest, DIFT_START ) {
    InSequence dummy;
    phys_ram_size = 0x300000000;
    GEN_MOCK_OBJECT( mock );
    EXPECT_CALL(mock, fopen(_,_)).WillOnce(Return((FILE*)0x1));
    EXPECT_CALL(mock, pre_generate_routine());
    EXPECT_CALL(mock, init_queue());
    EXPECT_CALL(mock, init_case_mapping());
    EXPECT_CALL(mock, init_dift_context(_));
    EXPECT_CALL(mock, pthread_create(_,_,_,_)).WillOnce(Return(0));

    int dift_start_code = dift_start();
    ASSERT_EQ( dift_start_code, DIFT_SUCCESS );
}

TEST( DiftUnitTest, DIFT_START_FOPEN_ERROR ) {
    ::testing::internal::CaptureStderr();
    char error_message[30];
    bzero( error_message, 30 );

    GEN_MOCK_OBJECT( mock );
    EXPECT_CALL(mock, fopen(_,_)).WillOnce(ReturnNull());
    EXPECT_CALL(mock, pre_generate_routine()).Times(0);
    EXPECT_CALL(mock, init_queue()).Times(0);
    EXPECT_CALL(mock, init_case_mapping()).Times(0);

    int dift_start_code = dift_start();
    ASSERT_EQ(dift_start_code, DIFT_ERR_FAIL );
    strcpy( error_message, ::testing::internal::GetCapturedStderr().c_str() );
    ASSERT_STREQ( error_message, "Fail to create DIFT log file\n" );
}

TEST( DiftUnitTest, DIFT_START_PTHREAD_ERROR ) {
    char error_message[38];
    FILE *readLog;
    bzero( error_message, 38 );

    GEN_MOCK_OBJECT( mock );
    EXPECT_CALL(mock, fopen(_,_)).WillOnce(Invoke(fopen));
    EXPECT_CALL(mock, pre_generate_routine());
    EXPECT_CALL(mock, init_queue());
    EXPECT_CALL(mock, init_case_mapping());
    EXPECT_CALL(mock, init_dift_context(_));
    EXPECT_CALL(mock, pthread_create(_,_,_,_)).WillOnce(Return(EAGAIN));
    EXPECT_CALL(mock, dift_log(_)).WillOnce(Invoke(_dift_log));
    EXPECT_CALL(mock, vfprintf(_,_,_)).WillOnce(Invoke(vfprintf));
    EXPECT_CALL(mock, fflush(_)).WillOnce(Invoke(fflush));

    int dift_start_code = dift_start();
    ASSERT_EQ(dift_start_code, DIFT_ERR_FAIL );

    readLog = fopen( "dift.log", "r" );
    fread( error_message, sizeof(char), 38, readLog );
    fclose( readLog );
    ASSERT_STREQ( error_message, "Fail to startup DIFT analysis thread\n" );
}

TEST( DiftUnitTest, DIFT_COMTAMINATE_MEM_OR_IS_VALID_TAG_ERROR ) {
    int dift_code = DIFT_SUCCESS;
    GEN_MOCK_OBJECT( mock );
    
    phys_ram_size = 0x100;
    EXPECT_CALL(mock, is_valid_mem_range(_,_)).WillRepeatedly(Return(true));
    EXPECT_CALL(mock, is_valid_disk_range(_,_)).WillRepeatedly(Return(true));
    EXPECT_CALL(mock, is_valid_tag(_)).WillRepeatedly(Invoke(_is_valid_tag));
    
    dift_code = dift_contaminate_memory_or(0x0, 0x1, 0x80);
    ASSERT_EQ( dift_code, DIFT_ERR_INVALIDTAG );
    dift_code = DIFT_SUCCESS;

    dift_code = dift_contaminate_memory_and(0x0, 0x1, 0x80);
    ASSERT_EQ( dift_code, DIFT_ERR_INVALIDTAG );
    dift_code = DIFT_SUCCESS;

    dift_code = dift_contaminate_disk_or(0x0, 0x1, 0x80);
    ASSERT_EQ( dift_code, DIFT_ERR_INVALIDTAG );
    dift_code = DIFT_SUCCESS;

    dift_code = dift_contaminate_disk_and(0x0, 0x1, 0x80);
    ASSERT_EQ( dift_code, DIFT_ERR_INVALIDTAG );
}

TEST( DiftUnitTest, DIFT_COMTAMINATE_IS_VALID_MEM_ERROR ) {
    int dift_code = 0;
    GEN_MOCK_OBJECT( mock );
    
    phys_ram_size = 0x100;
    EXPECT_CALL(mock, is_valid_tag(_)).WillRepeatedly(Return(true));
    EXPECT_CALL(mock, is_valid_mem_range(_,_)).WillRepeatedly(Return(false));

    dift_code = dift_contaminate_memory_or(0x0, 99999, 0x0);
    ASSERT_EQ( dift_code, DIFT_ERR_OUTOFRANGE );  
    dift_code = 0;    

    dift_code = dift_contaminate_memory_and(0x0, 9999, 0x0);
    ASSERT_EQ( dift_code, DIFT_ERR_OUTOFRANGE );
    dift_code = 0;
}

TEST( DiftUnitTest, DIFT_COMTAMINATE_IS_VALID_HD_ERROR ) {
    int dift_code = 0;
    GEN_MOCK_OBJECT( mock );
    
    phys_ram_size = 0x100;
    EXPECT_CALL(mock, is_valid_tag(_)).WillRepeatedly(Return(true));
    EXPECT_CALL(mock, is_valid_disk_range(_,_)).WillRepeatedly(Return(false)); 

    dift_code = dift_contaminate_disk_or(0x0, 9999, 0x0);
    ASSERT_EQ( dift_code, DIFT_ERR_OUTOFRANGE );
    dift_code = 0;

    dift_code = dift_contaminate_disk_and(0x0, 9999, 0x0);
    ASSERT_EQ( dift_code, DIFT_ERR_OUTOFRANGE );
}
