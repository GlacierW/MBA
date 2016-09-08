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

#include <gmock/gmock.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <map>
#include <inttypes.h>
#include <stdint.h>

struct dift_context;
struct hook_functions {
    virtual ~hook_functions() {};

    virtual void exit(int) = 0;
    virtual size_t getpagesize() = 0;
    virtual void* calloc(size_t, size_t) = 0;
    virtual int dift_log(char const *) = 0;
    virtual int fflush(FILE *) = 0;
    virtual int vfprintf(FILE *, const char *, va_list) = 0;
    virtual FILE* fopen(const char *, const char *) = 0;
    virtual void dift_rec_enqueue(uint64_t) = 0;
    virtual int is_valid_tag(const uint8_t) = 0;
    virtual int is_valid_mem_range(uint64_t, uint64_t) = 0;
    virtual int is_valid_disk_range(uint64_t, uint64_t) = 0;
    virtual void pre_generate_routine() = 0;
    virtual void init_queue() = 0;
    virtual void init_case_mapping() = 0;
    virtual void init_dift_context(dift_context*) = 0;
    virtual int pthread_create(pthread_t*, const pthread_attr_t*, void*(*)(void*), void* ) = 0;
    //virtual int pthread_create(...) = 0;
};
struct mock_functions : hook_functions {
    MOCK_METHOD1(exit, void(int));
    MOCK_METHOD0(getpagesize, size_t());
    MOCK_METHOD2(calloc, void*(size_t, size_t));
    MOCK_METHOD1(dift_log, int(char const *));
    MOCK_METHOD1(fflush, int(FILE *));
    MOCK_METHOD3(vfprintf, int(FILE *, const char *, va_list));
    MOCK_METHOD2(fopen, FILE*(const char *, const char * ));
    MOCK_METHOD1(dift_rec_enqueue, void(uint64_t));
    MOCK_METHOD1(is_valid_tag, int(const uint8_t));
    MOCK_METHOD2(is_valid_mem_range, int(uint64_t, uint64_t));
    MOCK_METHOD2(is_valid_disk_range, int(uint64_t, uint64_t));
    MOCK_METHOD0(pre_generate_routine, void());
    MOCK_METHOD0(init_queue, void());
    MOCK_METHOD0(init_case_mapping, void());
    MOCK_METHOD1(init_dift_context, void(dift_context*));
   //  MOCK_METHOD1(pthread_create, int(...));
    MOCK_METHOD4(pthread_create, int(pthread_t*, const pthread_attr_t*, void*(*)(void*), void*));
} *mock_ptr;

extern "C" {
#include "../dift.c"
}
#undef exit
#undef getpagesize
#undef calloc
#undef dift_log
#undef fflush
#undef vfprintf
#undef fopen
#undef dift_rec_enqueue
#undef is_valid_tag
#undef is_valid_mem_range
#undef is_valid_disk_range
#undef pre_generate_routine
#undef init_queue
#undef init_case_mapping
#undef init_dift_context
#undef pthread_create

#define GEN_MOCK_OBJECT(x) mock_functions x; mock_ptr = &x;
using namespace ::testing;
TEST( DiftUnitTest, DIFT_CONTEXT_INITILIZATION_NORMAL ) {
    // arrange
    GEN_MOCK_OBJECT( mock );
    phys_ram_size = 0x10000;
    size_t const pagesize = getpagesize();
    
    EXPECT_CALL(mock, calloc(pagesize + phys_ram_size, _))
    .Times(1).WillOnce(Invoke(calloc));
    EXPECT_CALL(mock, calloc(1<<HD_L1_INDEX_BITS, _))
    .Times(1).WillOnce(Invoke(calloc));

    EXPECT_CALL(mock, getpagesize()).Times(1).WillOnce(Return(pagesize));
    
    // act
    _init_dift_context( dc );
    // assert

    size_t count_checked_bytes = 0;
    #define ASSERT_EQ_ACCU_SIZE(expect, real)\
        do {\
            ASSERT_EQ( expect, real );\
            count_checked_bytes += sizeof(real);\
        } while(false)

    ASSERT_EQ_ACCU_SIZE( NULL, dc->deqptr );
    ASSERT_EQ_ACCU_SIZE( 0, dc->tail );
    ASSERT_EQ_ACCU_SIZE( Q_CHUNKS_SIZE - 1, dc->prev_tail );
    for( int i = 0; i < 24; ++i )
        for( int j = 0; j < 8; ++j )
            ASSERT_EQ_ACCU_SIZE( 0, dc->reg_dirty_tbl[i][j] );
    for( int i = 0; i < 16; ++i )
        for( int j = 0; j < 16; ++j )
            ASSERT_EQ_ACCU_SIZE( 0, dc->xmm_dirty_tbl[i][j] );
    for( int i = 0; i < (int)phys_ram_size; ++i ) 
        ASSERT_EQ( 0, dc->mem_dirty_tbl[i] );
    count_checked_bytes += sizeof(dc->mem_dirty_tbl);
    for( int i = 0; i < HD_L1_INDEX_BITS; ++i )
        ASSERT_EQ( NULL, dc->hd_l1_dirty_tbl[i] );
    count_checked_bytes += sizeof(dc->hd_l1_dirty_tbl);

    ASSERT_EQ(sizeof(dift_context), count_checked_bytes);
    #undef ASSERT_EQ_ACCU_SIZE
}

TEST( DiftUnitTest, DIFT_CONTEXT_INITILIZATION_ERROR ) {
// arrange
    int exception = 1;
    InSequence dummy;
    GEN_MOCK_OBJECT( mock );

    EXPECT_CALL(mock, exit(Ne(1))).Times(0);

    EXPECT_CALL(mock, calloc(_, _))
        .WillOnce(Invoke(calloc)).WillOnce(ReturnNull());
    EXPECT_CALL(mock, dift_log(_));
    EXPECT_CALL(mock, exit(1)).WillOnce(Throw(exception));

    EXPECT_CALL(mock, calloc(_, _)).WillOnce(ReturnNull());
    EXPECT_CALL(mock, dift_log(_));
    EXPECT_CALL(mock, exit(1)).WillOnce(Throw(exception));

    ASSERT_ANY_THROW({
        _init_dift_context(dc);
    });
    ASSERT_ANY_THROW({
        _init_dift_context(dc);
    });
}

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
    CONTAMINATION_RECORD* rec = alloc_hd_dirty_page();
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

    CONTAMINATION_RECORD* rec = alloc_hd_dirty_page();
    ASSERT_EQ( NULL, rec ); // make sure rec is empty
}


TEST( DiftUnitTest, DIFT_ENABLE ) {
    bool temp = dift_is_enabled();
    dift_enable();
    ASSERT_EQ( true, dift_is_enabled() );
    dift_enabled = temp;
}

TEST( DiftUnitTest, DIFT_DISABLE ) {
    bool temp = dift_is_enabled();
    dift_disable();
    ASSERT_EQ( false, dift_is_enabled() );
    dift_enabled = temp;
}

TEST( DiftUnitTest, DIFT_IS_ENABLED ) {
    bool temp = dift_is_enabled();
    dift_enable();
    ASSERT_EQ( true, dift_is_enabled() );
    dift_disable();
    ASSERT_EQ( false, dift_is_enabled() );
    dift_enabled = temp;
}

TEST( DiftUnitTest, DIFT_GET_MEMORY_DIRTY ) {
    phys_ram_base = 0x0;
    phys_ram_size = 0x10000;

    GEN_MOCK_OBJECT( mock ); 
    EXPECT_CALL(mock, calloc(_,_)).WillRepeatedly(Invoke(calloc));
    EXPECT_CALL(mock, getpagesize()).WillRepeatedly(Invoke(getpagesize));
    
    _init_dift_context( dc );    
    ASSERT_EQ( 0x0, get_mem_dirty(dc, 0x0000000000000000) );
}

TEST( DiftUnitTest, DIFT_GET_HD_DIRTY ) {
    GEN_MOCK_OBJECT( mock );
    EXPECT_CALL(mock, calloc(_,_)).WillRepeatedly(Invoke(calloc));
    EXPECT_CALL(mock, getpagesize()).WillRepeatedly(Invoke(getpagesize));
    _init_dift_context( dc );
    ASSERT_EQ( 0x0, get_hd_dirty(dc, 0x0000000000000000) );
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
    EXPECT_CALL(mock, fopen(_,_)).WillOnce(Invoke(fopen));
    EXPECT_CALL(mock, pre_generate_routine()).WillOnce(Invoke(_pre_generate_routine));
    EXPECT_CALL(mock, init_queue()).WillOnce(Invoke(_init_queue));
    EXPECT_CALL(mock, init_case_mapping()).WillOnce(Invoke(_init_case_mapping));
    EXPECT_CALL(mock, init_dift_context(_)).WillOnce(Invoke(_init_dift_context));
    EXPECT_CALL(mock, pthread_create(_,_,_,_)).WillOnce(Invoke(pthread_create));    

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

    const int dift_err_fail = 3;
    int dift_start_code = dift_start();
    ASSERT_EQ(dift_start_code, dift_err_fail );
    strcpy( error_message, ::testing::internal::GetCapturedStderr().c_str() );
    ASSERT_STREQ( error_message, "Fail to create DIFT log file\n" );
}

TEST( DiftUnitTest, DIFT_COMTAMINATE_MEM_OR ) {
    InSequence dummy;
    phys_ram_size = 0x300000000;
    CONTAMINATION_RECORD tag = 0x0;
    uint64_t len_pt = 1, len_max = 0xffffffff, one = 0x0, two = 0xffffffff, three = 0x1fffffffe;
    GEN_MOCK_OBJECT( mock ); 
    dift_record recTemp = { 0 }, recFirst = { 0 }, recSecond = { 0 };
    recTemp.case_nb = REC_CONTAMINATE_MEM_OR;

    *((uint64_t*)&recTemp) |= ((0x00000000000000ff & tag ) << 8 );
    *((uint64_t*)&recTemp) |= ((0x00000000ffffffff & len_max ) << 16 ); 
    memcpy( &recFirst, &recTemp, sizeof(recTemp)); 

    *((uint64_t*)&recTemp) |= ((0x00000000000000ff & tag ) << 8 );
    *((uint64_t*)&recTemp) |= ((0x00000000ffffffff & len_max ) << 16 );
    memcpy( &recSecond, &recTemp, sizeof(recTemp));
   
    *((uint64_t*)&recTemp) |= ((0x00000000000000ff & tag ) << 8 );
    *((uint64_t*)&recTemp) |= ((0x00000000ffffffff & len_pt ) << 16 );
   
    EXPECT_CALL(mock, fopen(_,_)).WillOnce(Invoke(fopen));
    EXPECT_CALL(mock, getpagesize()).WillRepeatedly(Invoke(getpagesize));
    EXPECT_CALL(mock, calloc(_,_)).WillRepeatedly(Invoke(calloc)); 
    EXPECT_CALL(mock, is_valid_tag(_)).WillOnce(Invoke(_is_valid_tag));
    EXPECT_CALL(mock, is_valid_mem_range(_,_)).WillOnce(Invoke(_is_valid_mem_range));
    EXPECT_CALL(mock, dift_rec_enqueue(*((uint64_t*)&recFirst))).Times(1);
    EXPECT_CALL(mock, dift_rec_enqueue(one)).Times(1);
    EXPECT_CALL(mock, dift_rec_enqueue(*((uint64_t*)&recSecond))).Times(1);
    EXPECT_CALL(mock, dift_rec_enqueue(two)).Times(1);
    EXPECT_CALL(mock, dift_rec_enqueue(*((uint64_t*)&recTemp))).Times(1);
    EXPECT_CALL(mock, dift_rec_enqueue(three)).Times(1);                     

    dift_start();
    int dift_code = dift_contaminate_memory_or(0x0, 0x1ffffffff, tag );
    ASSERT_EQ( dift_code, 0 );
}

TEST( DiftUnitTest, DIFT_COMTAMINATE_MEM_AND ) {
    InSequence dummy;
    phys_ram_size = 0x300000000;
    CONTAMINATION_RECORD tag = 0x0;
    uint64_t len_pt = 1, len_max = 0xffffffff, one = 0x0, two = 0xffffffff, three = 0x1fffffffe;
    GEN_MOCK_OBJECT( mock ); 
    dift_record recTemp = { 0 }, recFirst = { 0 }, recSecond = { 0 };
    recTemp.case_nb = REC_CONTAMINATE_MEM_AND;

    *((uint64_t*)&recTemp) |= ((0x00000000000000ff & tag ) << 8 );
    *((uint64_t*)&recTemp) |= ((0x00000000ffffffff & len_max ) << 16 ); 
    memcpy( &recFirst, &recTemp, sizeof(recTemp)); 

    *((uint64_t*)&recTemp) |= ((0x00000000000000ff & tag ) << 8 );
    *((uint64_t*)&recTemp) |= ((0x00000000ffffffff & len_max ) << 16 );
    memcpy( &recSecond, &recTemp, sizeof(recTemp));
   
    *((uint64_t*)&recTemp) |= ((0x00000000000000ff & tag ) << 8 );
    *((uint64_t*)&recTemp) |= ((0x00000000ffffffff & len_pt ) << 16 );
  
    EXPECT_CALL(mock, fopen(_,_)).WillOnce(Invoke(fopen)); 
    EXPECT_CALL(mock, getpagesize()).WillRepeatedly(Invoke(getpagesize));
    EXPECT_CALL(mock, calloc(_,_)).WillRepeatedly(Invoke(calloc)); 
    EXPECT_CALL(mock, is_valid_tag(_)).WillOnce(Invoke(_is_valid_tag));
    EXPECT_CALL(mock, is_valid_mem_range(_,_)).WillOnce(Invoke(_is_valid_mem_range));
    EXPECT_CALL(mock, dift_rec_enqueue(*((uint64_t*)&recFirst))).Times(1);
    EXPECT_CALL(mock, dift_rec_enqueue(one)).Times(1);
    EXPECT_CALL(mock, dift_rec_enqueue(*((uint64_t*)&recSecond))).Times(1);
    EXPECT_CALL(mock, dift_rec_enqueue(two)).Times(1);
    EXPECT_CALL(mock, dift_rec_enqueue(*((uint64_t*)&recTemp))).Times(1);
    EXPECT_CALL(mock, dift_rec_enqueue(three)).Times(1);                     

    dift_start();
    int dift_code = dift_contaminate_memory_and(0x0, 0x1ffffffff, tag );
    ASSERT_EQ( dift_code, 0 );
}
  
TEST( DiftUnitTest, DIFT_COMTAMINATE_HD_OR ) {
    InSequence dummy;
    phys_ram_size = 0x300000000;
    CONTAMINATION_RECORD tag = 0x0;
    const uint64_t len = 0x10, haddr = 0x0;
    GEN_MOCK_OBJECT( mock ); 
    dift_record recTemp = { 0 };
    recTemp.case_nb = REC_CONTAMINATE_HD_OR;
    *((uint64_t*)&recTemp) |= ((0x00000000000000ff & tag ) << 8 );
      
    EXPECT_CALL(mock, fopen(_,_)).WillOnce(Invoke(fopen));
    EXPECT_CALL(mock, getpagesize()).WillRepeatedly(Invoke(getpagesize));
    EXPECT_CALL(mock, calloc(_,_)).WillRepeatedly(Invoke(calloc)); 
    EXPECT_CALL(mock, is_valid_tag(_)).WillOnce(Invoke(_is_valid_tag));
    EXPECT_CALL(mock, is_valid_disk_range(_,_)).WillOnce(Invoke(_is_valid_disk_range));
    EXPECT_CALL(mock, dift_rec_enqueue(*((uint64_t*)&recTemp))).Times(1);
    EXPECT_CALL(mock, dift_rec_enqueue(haddr)).Times(1);
    EXPECT_CALL(mock, dift_rec_enqueue(len)).Times(1);    

    dift_start();
    int dift_code = dift_contaminate_disk_or(haddr, len, tag );
    ASSERT_EQ( dift_code, 0 );
}

TEST( DiftUnitTest, DIFT_COMTAMINATE_HD_AND ) {
    InSequence dummy;
    phys_ram_size = 0x300000000;
    CONTAMINATION_RECORD tag = 0x0;
    const uint64_t len = 0x10, haddr = 0x0;
    GEN_MOCK_OBJECT( mock ); 
    dift_record recTemp = { 0 };
    recTemp.case_nb = REC_CONTAMINATE_HD_AND;
    *((uint64_t*)&recTemp) |= ((0x00000000000000ff & tag ) << 8 );
      
    EXPECT_CALL(mock, fopen(_,_)).WillOnce(Invoke(fopen));
    EXPECT_CALL(mock, getpagesize()).WillRepeatedly(Invoke(getpagesize));
    EXPECT_CALL(mock, calloc(_,_)).WillRepeatedly(Invoke(calloc)); 
    EXPECT_CALL(mock, is_valid_tag(_)).WillOnce(Invoke(_is_valid_tag));
    EXPECT_CALL(mock, is_valid_disk_range(_,_)).WillOnce(Invoke(_is_valid_disk_range));
    EXPECT_CALL(mock, dift_rec_enqueue(*((uint64_t*)&recTemp))).Times(1);
    EXPECT_CALL(mock, dift_rec_enqueue(haddr)).Times(1);
    EXPECT_CALL(mock, dift_rec_enqueue(len)).Times(1);    

    dift_start();
    int dift_code = dift_contaminate_disk_and(haddr, len, tag );
    ASSERT_EQ( dift_code, 0 );
}

TEST( DiftUnitTest, DIFT_COMTAMINATE_MEM_OR_IS_VALID_TAG_ERROR ) {
    phys_ram_size = 0x100;
    int dift_code = 0;
    GEN_MOCK_OBJECT( mock );
    EXPECT_CALL(mock, calloc(_,_)).WillRepeatedly(Invoke(calloc));
    EXPECT_CALL(mock, getpagesize()).WillRepeatedly(Invoke(getpagesize));
    EXPECT_CALL(mock, dift_rec_enqueue(_)).Times(0);
    EXPECT_CALL(mock, is_valid_tag(_)).WillRepeatedly(Return(false));
    EXPECT_CALL(mock, is_valid_mem_range(_,_)).Times(0);
    EXPECT_CALL(mock, is_valid_disk_range(_,_)).Times(0);
    dift_code = dift_contaminate_memory_or(0x0,99999, 0x0);
    ASSERT_EQ( dift_code, 2 );
    dift_code = 0;

    dift_code = dift_contaminate_memory_and(0x0,99999, 0x0);
    ASSERT_EQ( dift_code, 2 );
    dift_code = 0;

    dift_code = dift_contaminate_disk_or(0x0,99999, 0x0);
    ASSERT_EQ( dift_code, 2 );
    dift_code = 0;

    dift_code = dift_contaminate_disk_and(0x0,99999, 0x0);
    ASSERT_EQ( dift_code, 2 );
}


TEST( DiftUnitTest, DIFT_COMTAMINATE_IS_VALID_MEM_ERROR ) {
    phys_ram_size = 0x100;
    int dift_code = 0;
    GEN_MOCK_OBJECT( mock );
    EXPECT_CALL(mock, calloc(_,_)).WillRepeatedly(Invoke(calloc));
    EXPECT_CALL(mock, getpagesize()).WillRepeatedly(Invoke(getpagesize));
    EXPECT_CALL(mock, dift_rec_enqueue(_)).Times(0);
    EXPECT_CALL(mock, is_valid_tag(_)).WillRepeatedly(Return(true));
    EXPECT_CALL(mock, is_valid_mem_range(_,_)).WillRepeatedly(Return(false));
    dift_code = dift_contaminate_memory_or(0x0, 99999, 0x0);
    ASSERT_EQ( dift_code, 1 );  
    dift_code = 0;    

    dift_code = dift_contaminate_memory_and(0x0, 9999, 0x0);
    ASSERT_EQ( dift_code, 1 );
    dift_code = 0;
}

TEST( DiftUnitTest, DIFT_COMTAMINATE_IS_VALID_HD_ERROR ) {
    phys_ram_size = 0x100;
    int dift_code = 0;
    GEN_MOCK_OBJECT( mock );
    EXPECT_CALL(mock, calloc(_,_)).WillRepeatedly(Invoke(calloc));
    EXPECT_CALL(mock, getpagesize()).WillRepeatedly(Invoke(getpagesize));
    EXPECT_CALL(mock, dift_rec_enqueue(_)).Times(0);
    EXPECT_CALL(mock, is_valid_tag(_)).WillRepeatedly(Return(true));
    EXPECT_CALL(mock, is_valid_disk_range(_,_)).WillRepeatedly(Return(false)); 

    dift_code = dift_contaminate_disk_or(0x0, 9999, 0x0);
    ASSERT_EQ( dift_code, 1 );
    dift_code = 0;

    dift_code = dift_contaminate_disk_and(0x0, 9999, 0x0);
    ASSERT_EQ( dift_code, 1 );
}


