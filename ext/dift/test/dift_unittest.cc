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

struct hook_functions {
    virtual ~hook_functions() {};

    virtual void exit(int) = 0;
    virtual size_t getpagesize() = 0;
    virtual void* calloc(size_t, size_t) = 0;
    virtual int dift_log(char const *) = 0;
};
struct mock_functions : hook_functions {
    MOCK_METHOD1(exit, void(int));
    MOCK_METHOD0(getpagesize, size_t());
    MOCK_METHOD2(calloc, void*(size_t, size_t));
    MOCK_METHOD1(dift_log, int(char const *));
} *mock_ptr;

extern "C" {
#include "../dift.c"
}
#undef exit
#undef getpagesize
#undef calloc
#undef dift_log

#define GEN_MOCK_OBJECT(x)  mock_functions x; mock_ptr = &mock;

TEST( DiftUnitTest, DIFT_CONTEXT_INITILIZATION_NORMAL ) {
    using namespace ::testing;
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
    init_dift_context( dc );
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
    using namespace ::testing;
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
        init_dift_context(dc);
    });
    ASSERT_ANY_THROW({
        init_dift_context(dc);
    });
}
