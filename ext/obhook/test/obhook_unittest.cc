/*
 *  Out-of-Box Hooking test cases
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

// re-define certain header in order to decouple the QEMU dependency
#include "../../../include/utlist.h"
#include "../../../include/uthash.h"
typedef uint64_t target_ulong;

struct hook_func {

    virtual ~hook_func() {};

    // library API
    virtual void* calloc( size_t, size_t ) = 0;

    // obhook functions/API
    virtual int  get_obhook_descriptor( void ) = 0;
};

struct mock_func : hook_func {
    public:
        MOCK_METHOD2( calloc, void*(size_t, size_t) );
        MOCK_METHOD0( get_obhook_descriptor, int() );

} *mock_ptr;

#define GEN_MOCK_OBJECT(x)  mock_func x; mock_ptr = &x;

// mock the functions in the original code
#define calloc(x, y)             mock_ptr->calloc(x, y)
#define get_obhook_descriptor(x) mock_ptr->get_obhook_descriptor(x)

extern "C" {
#include "../obhook.c"
}

#undef calloc
#undef get_obhook_descriptor

// dummy callback for the obhook registration test
void* dummy_cb( void* arg ) {
    return NULL;
}

using namespace ::testing;

TEST( GetUidTest, NormalCondition ) {
    ASSERT_NE( -1, _get_obhook_descriptor() );
}
TEST( GetUidTest, FullCondition ) {

    void* dummy = (void*)0xffffffffffffffff;

    // exhausting available UID
    for( int i = 0; i < MAX_NM_OBHOOK; ++i ) 
        obhk_ctx->index_tbl[i] = (obhk_cb_record*)dummy;
    // hook space should be (simulated) full now
    ASSERT_EQ( -1, _get_obhook_descriptor() );

    // restore shared resource
    for( int i = 0; i < MAX_NM_OBHOOK; ++i ) 
       obhk_ctx->index_tbl[i] = NULL;
}

TEST( KernelAddrCheckTest, ValidKernelAddress ) {
    ASSERT_TRUE( is_kern_addr(0xffffffffffffffff) );
    ASSERT_TRUE( is_kern_addr(0xffff0000ffff0000) );

}
TEST( KernelAddrCheckTest, InvalidKernelAddress ) {
    ASSERT_FALSE( is_kern_addr(0x1234567812345678) );
    ASSERT_FALSE( is_kern_addr(0x00007ffeb1234567) );
}

TEST( AddUnivHookTest, FullHookErrorHandle ) {
    GEN_MOCK_OBJECT( mock );
    EXPECT_CALL( mock, get_obhook_descriptor() ).WillOnce( Return(-1) );
    ASSERT_EQ( -1, obhook_add_universal(0xffffffffffffffff, "label", dummy_cb) );
    ASSERT_EQ( OBHOOK_ERR_FULL_HOOK, obhook_errno );
}
TEST( AddUnivHookTest, InvalidAddressErrorHandle ) {
    GEN_MOCK_OBJECT( mock );
    EXPECT_CALL( mock, get_obhook_descriptor() ).WillOnce( Invoke(_get_obhook_descriptor) );
    ASSERT_EQ( -1, obhook_add_universal(0x0000000000000000, "label", dummy_cb) );
    ASSERT_EQ( OBHOOK_ERR_INVALID_ADDR, obhook_errno );
}
TEST( AddUnivHookTest, LongLabelErrorHandle ) {
    GEN_MOCK_OBJECT( mock );
    EXPECT_CALL( mock, get_obhook_descriptor() ).WillOnce( Invoke(_get_obhook_descriptor) );
    ASSERT_EQ( -1, obhook_add_universal(0xffffffffffffffff, "toooooooooooooo_long_label", dummy_cb) );
    ASSERT_EQ( OBHOOK_ERR_INVALID_LABEL, obhook_errno );
}
TEST( AddUnivHookTest, InvalidCallbackErrorHandle ) {
    GEN_MOCK_OBJECT( mock );
    EXPECT_CALL( mock, get_obhook_descriptor() ).WillOnce( Invoke(_get_obhook_descriptor) );
    ASSERT_EQ( -1, obhook_add_universal(0xffffffffffffffff, "label", NULL) );
    ASSERT_EQ( OBHOOK_ERR_INVALID_CALLBACK, obhook_errno );
}
TEST( AddUnivHookTest, MemoryErrorHandle ) {
    GEN_MOCK_OBJECT( mock );
    EXPECT_CALL( mock, get_obhook_descriptor() ).WillOnce( Invoke(_get_obhook_descriptor) );
    EXPECT_CALL( mock, calloc(_,_) ).WillOnce( ReturnNull() );
    ASSERT_EQ( -1, obhook_add_universal(0xffffffffffffffff, "label", dummy_cb) );
    ASSERT_EQ( OBHOOK_ERR_FAIL, obhook_errno );

    EXPECT_CALL( mock, get_obhook_descriptor() ).WillOnce( Invoke(_get_obhook_descriptor) );
    EXPECT_CALL( mock, calloc(_,_) ).Times(2).WillOnce( Invoke(calloc) ).WillOnce( ReturnNull() );
    ASSERT_EQ( -1, obhook_add_universal(0xffffffffffffffff, "label", dummy_cb) );
    ASSERT_EQ( OBHOOK_ERR_FAIL, obhook_errno );
}
TEST( AddUnivHookTest, NormalCondition ) {
    GEN_MOCK_OBJECT( mock );
    EXPECT_CALL( mock, get_obhook_descriptor() ).WillOnce( Invoke(_get_obhook_descriptor) );
    EXPECT_CALL( mock, calloc(_,_) ).WillRepeatedly( Invoke(calloc) );
    ASSERT_EQ( 0, obhook_add_universal(0xffffffffffffffff, "label", dummy_cb) );
}

TEST( AddProcHookTest, InvalidCR3ErrorHandle ) {
    GEN_MOCK_OBJECT( mock );
    ASSERT_EQ( -1, obhook_add_process(0, 0xffffffffffffffff, "label", dummy_cb) );
    ASSERT_EQ( OBHOOK_ERR_INVALID_CR3, obhook_errno );
}
TEST( AddProcHookTest, FullHookErrorHandle ) {
    GEN_MOCK_OBJECT( mock );
    EXPECT_CALL( mock, get_obhook_descriptor() ).WillOnce( Return(-1) );
    ASSERT_EQ( -1, obhook_add_process(0x1234567812345678, 0xffffffffffffffff, "label", dummy_cb) );
    ASSERT_EQ( OBHOOK_ERR_FULL_HOOK, obhook_errno );
}
TEST( AddProcHookTest, LongLabelErrorHandle ) {
    GEN_MOCK_OBJECT( mock );
    EXPECT_CALL( mock, get_obhook_descriptor() ).WillOnce( Invoke(_get_obhook_descriptor) );
    ASSERT_EQ( -1, obhook_add_process(0x1234567812345678, 0xffffffffffffffff, "toooooooooooooo_long_label", dummy_cb) );
    ASSERT_EQ( OBHOOK_ERR_INVALID_LABEL, obhook_errno );
}
TEST( AddProcHookTest, InvalidCallbackErrorHandle ) {
    GEN_MOCK_OBJECT( mock );
    EXPECT_CALL( mock, get_obhook_descriptor() ).WillOnce( Invoke(_get_obhook_descriptor) );
    ASSERT_EQ( -1, obhook_add_process(0x1234567812345678, 0xffffffffffffffff, "label", NULL) );
    ASSERT_EQ( OBHOOK_ERR_INVALID_CALLBACK, obhook_errno );
}
TEST( AddProcHookTest, MemoryErrorHandle ) {
    GEN_MOCK_OBJECT( mock );
    EXPECT_CALL( mock, get_obhook_descriptor() ).WillOnce( Invoke(_get_obhook_descriptor) );
    EXPECT_CALL( mock, calloc(_,_) ).WillOnce( ReturnNull() );
    ASSERT_EQ( -1, obhook_add_process(0x1234567812345678, 0xffffffffffffffff, "label", dummy_cb) );
    ASSERT_EQ( OBHOOK_ERR_FAIL, obhook_errno );

    EXPECT_CALL( mock, get_obhook_descriptor() ).WillOnce( Invoke(_get_obhook_descriptor) );
    EXPECT_CALL( mock, calloc(_,_) ).Times(2).WillOnce( Invoke(calloc) ).WillOnce( ReturnNull() );
    ASSERT_EQ( -1, obhook_add_process(0x1234567812345678, 0xffffffffffffffff, "label", dummy_cb) );
    ASSERT_EQ( OBHOOK_ERR_FAIL, obhook_errno );
}
TEST( AddProcHookTest, NormalConditionWithUserAddr ) {
    GEN_MOCK_OBJECT( mock );
    EXPECT_CALL( mock, get_obhook_descriptor() ).WillOnce( Invoke(_get_obhook_descriptor) );
    EXPECT_CALL( mock, calloc(_,_) ).WillRepeatedly( Invoke(calloc) );
    ASSERT_NE( -1, obhook_add_process(0x1234567812345678, 0x0000ffffffffffff, "label", dummy_cb) );
}
TEST( AddProcHookTest, NormalConditionWithKernAddr ) {
    GEN_MOCK_OBJECT( mock );
    EXPECT_CALL( mock, get_obhook_descriptor() ).WillOnce( Invoke(_get_obhook_descriptor) );
    EXPECT_CALL( mock, calloc(_,_) ).WillRepeatedly( Invoke(calloc) );
    ASSERT_NE( -1, obhook_add_process(0x1234567812345678, 0xffffffffffffffff, "label", dummy_cb) );
}

TEST( GetUnivHookTest, HookFound ) {
    
    GEN_MOCK_OBJECT( mock );

    int          hook_d   = -1;
    target_ulong addr  = 0xffff0000ffff0000;
    const char*  label = "label";

    obhk_cb_record* rec;

    // register a new universal hook
    EXPECT_CALL( mock, get_obhook_descriptor() ).WillOnce( Invoke(_get_obhook_descriptor) );
    EXPECT_CALL( mock, calloc(_,_) ).WillRepeatedly( Invoke(calloc) );
    hook_d = obhook_add_universal( addr, label, dummy_cb );
    ASSERT_NE( -1, hook_d );

    // get the hook & check record
    rec = obhook_getcbs_univ( addr );
    ASSERT_TRUE( rec != NULL );
    ASSERT_EQ( hook_d,  rec->uniq_d );
    ASSERT_TRUE( rec->universal );
    ASSERT_TRUE( rec->enabled );
    ASSERT_STREQ( label, rec->label );
}
TEST( GetUnivHookTest, HookNotFound ) {
    GEN_MOCK_OBJECT( mock );
    ASSERT_EQ( NULL, obhook_getcbs_univ(0xdeadbeefdeadbeed) );
}

TEST( GetProcHookTest, HookFound ) {
    
    GEN_MOCK_OBJECT( mock );

    int          hook_d = -1;
    target_ulong cr3    = 0x1234567812345678;
    target_ulong addr   = 0xffff0000ffff0000;
    const char*  label  = "proc_label";

    obhk_cb_record* rec;

    // register a new process hook
    EXPECT_CALL( mock, get_obhook_descriptor() ).WillOnce( Invoke(_get_obhook_descriptor) );
    EXPECT_CALL( mock, calloc(_,_) ).WillRepeatedly( Invoke(calloc) );
    hook_d = obhook_add_process( cr3, addr, label, dummy_cb );
    ASSERT_NE( -1, hook_d );

    // get the hook & check record
    rec = obhook_getcbs_proc( cr3, addr );
    ASSERT_TRUE( rec != NULL );
    ASSERT_EQ( hook_d,  rec->uniq_d );
    ASSERT_FALSE( rec->universal );
    ASSERT_TRUE( rec->enabled );
    ASSERT_STREQ( label, rec->label );

}
TEST( GetProcHookTest, HookNotFound ) {

    GEN_MOCK_OBJECT( mock );
    
    target_ulong cr3   = 0x8765432187654321;
    target_ulong addr  = 0xffff0000ffff0000;
    const char*  label = "proc_label";

    // register a new process hook
    EXPECT_CALL( mock, get_obhook_descriptor() ).WillOnce( Invoke(_get_obhook_descriptor) );
    EXPECT_CALL( mock, calloc(_,_) ).WillRepeatedly( Invoke(calloc) );
    ASSERT_NE( -1, obhook_add_process(cr3, addr, label, dummy_cb) );

    ASSERT_EQ( NULL, obhook_getcbs_proc(cr3 + 1, addr) );
    ASSERT_EQ( NULL, obhook_getcbs_proc(cr3, addr + 1) );
}
TEST( GetPorcHookTest, InvalidCR3ErrorHandle ) {
    ASSERT_EQ( NULL, obhook_getcbs_proc(0, 0x1234567812345678) );
    ASSERT_EQ( OBHOOK_ERR_INVALID_CR3, obhook_errno );
}

TEST( ToggleHookTest, HookDisableAndEnable ) {

    GEN_MOCK_OBJECT( mock );

    int          hook_d;
    target_ulong addr  = 0xffff0000ffff0001;
    const char*  label = "label";

    obhk_cb_record* rec;

    // register a new universal hook
    EXPECT_CALL( mock, get_obhook_descriptor() ).WillOnce( Invoke(_get_obhook_descriptor) );
    EXPECT_CALL( mock, calloc(_,_) ).WillRepeatedly( Invoke(calloc) );
    hook_d = obhook_add_universal( addr, label, dummy_cb );
    ASSERT_NE( -1, hook_d );

    // get the hook
    rec = obhook_getcbs_univ( addr );
    ASSERT_TRUE( rec != NULL );

    // disable hook
    ASSERT_EQ( 0, obhook_disable(hook_d) );
    ASSERT_FALSE( rec->enabled );

    // enable hook
    obhook_enable( hook_d );
    ASSERT_TRUE( rec->enabled );
}
TEST( ToggleHookTest, InvalidDescriptorErrorHandle ) {
    ASSERT_EQ( -1, obhook_enable(MAX_NM_OBHOOK) );
    ASSERT_EQ( OBHOOK_ERR_INVALID_DESCRIPTOR, obhook_errno );
    ASSERT_EQ( -1, obhook_enable(MAX_NM_OBHOOK+1) );
    ASSERT_EQ( OBHOOK_ERR_INVALID_DESCRIPTOR, obhook_errno );
    ASSERT_EQ( -1, obhook_enable(MAX_NM_OBHOOK-1) );
    ASSERT_EQ( OBHOOK_ERR_INVALID_DESCRIPTOR, obhook_errno );
    ASSERT_EQ( -1, obhook_enable(-1) );
    ASSERT_EQ( OBHOOK_ERR_INVALID_DESCRIPTOR, obhook_errno );
   
    ASSERT_EQ( -1, obhook_disable(MAX_NM_OBHOOK) );
    ASSERT_EQ( OBHOOK_ERR_INVALID_DESCRIPTOR, obhook_errno );
    ASSERT_EQ( -1, obhook_disable(MAX_NM_OBHOOK+1) );
    ASSERT_EQ( OBHOOK_ERR_INVALID_DESCRIPTOR, obhook_errno );
    ASSERT_EQ( -1, obhook_disable(MAX_NM_OBHOOK-1) );
    ASSERT_EQ( OBHOOK_ERR_INVALID_DESCRIPTOR, obhook_errno );
    ASSERT_EQ( -1, obhook_disable(-1) );
    ASSERT_EQ( OBHOOK_ERR_INVALID_DESCRIPTOR, obhook_errno );
}

TEST( DeleteHookTest, InvalidDescriptorErroHandle ) {
    ASSERT_EQ( -1, obhook_delete(MAX_NM_OBHOOK) );
    ASSERT_EQ( OBHOOK_ERR_INVALID_DESCRIPTOR, obhook_errno );
    ASSERT_EQ( -1, obhook_delete(MAX_NM_OBHOOK+1) );
    ASSERT_EQ( OBHOOK_ERR_INVALID_DESCRIPTOR, obhook_errno );
    ASSERT_EQ( -1, obhook_delete(MAX_NM_OBHOOK-1) );
    ASSERT_EQ( OBHOOK_ERR_INVALID_DESCRIPTOR, obhook_errno );
    ASSERT_EQ( -1, obhook_delete(-1) );
    ASSERT_EQ( OBHOOK_ERR_INVALID_DESCRIPTOR, obhook_errno );
}
TEST( DeleteHookTest, NormalCondition ) {
 
    int hook_d = -1;
    
    target_ulong cr3  = 0x8765432187654321;
    target_ulong addr = 0xffffffffffffffff;

    GEN_MOCK_OBJECT( mock );
    EXPECT_CALL( mock, get_obhook_descriptor() ).WillOnce( Invoke(_get_obhook_descriptor) );
    EXPECT_CALL( mock, calloc(_,_) ).WillRepeatedly( Invoke(calloc) );
    
    hook_d = obhook_add_process( cr3, addr, "to_be_deleted", dummy_cb );
    ASSERT_NE( -1, hook_d );
    ASSERT_TRUE( obhk_ctx->index_tbl[hook_d] != NULL );
    
    ASSERT_EQ( 0, obhook_delete(hook_d) );
    ASSERT_EQ( NULL, obhk_ctx->index_tbl[hook_d] );
}
