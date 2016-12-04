/*
 *  Agent Client (QEMU Part) test cases
 *
 *  Copyright (c) 2016 JuiChien, Jao 
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
#include <string>

using namespace std;

typedef void Monitor;

extern "C" {
#include "../agent.h"
}

struct hook_functions {
    virtual ~hook_functions() {};
    virtual int              call_back_int( void ) = 0;
    // Library Functions
    virtual void             exit( int ) = 0;
    virtual int              pthread_create(pthread_t*, const pthread_attr_t*, void*(*)(void*), void* ) = 0;
    virtual int              pthread_cancel(pthread_t) = 0;
    virtual int              pthread_join(pthread_t, void**) = 0;
    virtual int              pthread_mutex_init(pthread_mutex_t*, const pthread_mutexattr_t * ) = 0;
    virtual int              pthread_cond_init(pthread_cond_t*, const pthread_condattr_t*) = 0;
    virtual int              pthread_cond_signal(pthread_cond_t*) = 0;
    virtual int              pthread_cond_destroy(pthread_cond_t*) = 0;
    virtual int              pthread_mutex_lock(pthread_mutex_t*) = 0;
    virtual int              pthread_mutex_trylock(pthread_mutex_t*) = 0;
    virtual int              pthread_mutex_unlock(pthread_mutex_t*) = 0;
    virtual int              pthread_mutex_destroy(pthread_mutex_t*) = 0;
    virtual int              open( const char * ) = 0;
    virtual ssize_t          read( int, void*, size_t ) = 0;
    virtual ssize_t          write( int, void*, size_t ) = 0;
    virtual int              sendto( int, void*, int, int, struct sockaddr*, socklen_t ) = 0;
    virtual int              recvfrom( int, void*, int, int, struct sockaddr*, socklen_t* ) = 0;
    virtual int              stat( const char*, struct stat* ) = 0;
    virtual int              close( int ) = 0;
    virtual int              strncmp( const char*, const char*, ssize_t ) = 0;
    virtual int              socket( int, int, int ) = 0;
    virtual int              connect( int, const struct sockaddr*, socklen_t ) = 0;
    virtual void             monitor_vprintf( Monitor*, const char*, va_list ) = 0;
    // Static Functions
    virtual void             agent_cleanup( void ) = 0;
    virtual ssize_t          as_write( int, void*, size_t ) = 0;
    virtual ssize_t          as_read( int, void*, size_t ) = 0;
    virtual void             ecm_write( bool ) = 0;
    virtual bool             ecm_read( void ) = 0;
    virtual MBA_AGENT_RETURN set_agent_action( MBA_AGENT_ACTION ) = 0;
    virtual MBA_AGENT_RETURN import_host_file( void ) = 0;
    virtual MBA_AGENT_RETURN export_guest_file( void ) = 0;
    virtual MBA_AGENT_RETURN execute_guest_cmd_return( void ) = 0;
    virtual MBA_AGENT_RETURN execute_guest_cmd_noreturn( void ) = 0;
    virtual MBA_AGENT_RETURN export_agent_log( void ) = 0;
    virtual MBA_AGENT_RETURN sync_cache( void ) = 0;
    virtual int              connect_agent_server( void ) = 0;
    // Public API
    virtual void             agent_printf( const char* ) = 0;
    virtual MBA_AGENT_RETURN agent_init( Monitor*, uint16_t, int(*)(const char*)) = 0;
};

struct mock_functions : hook_functions {
    MOCK_METHOD0( call_back_int, int( void ) );
    // Library Functions
    MOCK_METHOD1( exit, void( int ) );
    MOCK_METHOD4( pthread_create, int(pthread_t*, const pthread_attr_t*, void*(*)(void*), void* ) );
    MOCK_METHOD1( pthread_cancel, int( pthread_t ) );
    MOCK_METHOD2( pthread_join, int( pthread_t, void** ) );
    MOCK_METHOD2( pthread_mutex_init, int( pthread_mutex_t*, const pthread_mutexattr_t* ));
    MOCK_METHOD2( pthread_cond_init, int( pthread_cond_t*, const pthread_condattr_t* ));
    MOCK_METHOD1( pthread_cond_signal, int( pthread_cond_t* ));
    MOCK_METHOD1( pthread_cond_destroy, int( pthread_cond_t* ));
    MOCK_METHOD1( pthread_mutex_lock, int( pthread_mutex_t* ) );
    MOCK_METHOD1( pthread_mutex_trylock, int( pthread_mutex_t* ) );
    MOCK_METHOD1( pthread_mutex_unlock, int( pthread_mutex_t* ) );
    MOCK_METHOD1( pthread_mutex_destroy, int( pthread_mutex_t* ) );
    MOCK_METHOD1( open, int(const char *) );
    MOCK_METHOD3( read, ssize_t(int, void*, size_t ) );
    MOCK_METHOD3( write, ssize_t(int, void*, size_t ) );
    MOCK_METHOD6( sendto, int( int, void*, int, int, struct sockaddr*, socklen_t ) );
    MOCK_METHOD6( recvfrom, int( int, void*, int, int, struct sockaddr*, socklen_t* ) );
    MOCK_METHOD2( stat, int( const char*, struct stat* ) );
    MOCK_METHOD1( close, int ( int ) );
    MOCK_METHOD3( strncmp, int( const char*, const char*, ssize_t ) );
    MOCK_METHOD3( socket, int( int, int, int ) );
    MOCK_METHOD3( connect, int( int, const struct sockaddr*, socklen_t ) ); 
    MOCK_METHOD3( monitor_vprintf, void( Monitor*, const char*, va_list ) );
    // Static Functions
    MOCK_METHOD0( agent_cleanup, void ( void ) );
    MOCK_METHOD3( as_write, ssize_t( int, void*, size_t ) );
    MOCK_METHOD3( as_read, ssize_t( int, void*, size_t ) );
    MOCK_METHOD1( ecm_write, void( bool ) );
    MOCK_METHOD0( ecm_read, bool( void ) );
    MOCK_METHOD1( set_agent_action, MBA_AGENT_RETURN( MBA_AGENT_ACTION ) );
    MOCK_METHOD0( import_host_file, MBA_AGENT_RETURN( void ) );
    MOCK_METHOD0( export_guest_file, MBA_AGENT_RETURN( void ) );
    MOCK_METHOD0( execute_guest_cmd_return, MBA_AGENT_RETURN( void ) );
    MOCK_METHOD0( execute_guest_cmd_noreturn, MBA_AGENT_RETURN( void ) );
    MOCK_METHOD0( export_agent_log, MBA_AGENT_RETURN( void ) );
    MOCK_METHOD0( sync_cache,  MBA_AGENT_RETURN( void ) );
    MOCK_METHOD0( connect_agent_server, int( void ) );
    // Public API
    MOCK_METHOD1( agent_printf, void(const char*) );
    MOCK_METHOD3( agent_init, MBA_AGENT_RETURN( Monitor*, uint16_t, int(*)(const char*)) );
} *mock_ptr;

#define FALSE 0
#define TRUE  1
extern "C" {
#include "../agent.c"
}

// Library Functions
#undef exit
#undef pthread_create
#undef pthread_cancel
#undef pthread_join
#undef pthread_mutex_init
#undef pthread_cond_init
#undef pthread_cond_signal
#undef pthread_cond_destroy
#undef pthread_mutex_lock
#undef pthread_mutex_trylock
#undef pthread_mutex_unlock
#undef pthread_mutex_destroy
#undef open
#undef read
#undef write
#undef sendto
#undef recvfrom
#undef stat
#undef close
#undef strncmp
#undef socket
#undef connect
#undef monitor_vprintf
// Static Functions
#undef agent_cleanup
#undef as_write
#undef as_read
#undef ecm_write
#undef ecm_read
#undef set_agent_action
#undef import_host_file
#undef export_guest_file
#undef execute_guest_cmd_return
#undef execute_guest_cmd_noreturn
#undef export_agent_log
#undef sync_cache
#undef connect_agent_server
// Public API
#undef agent_printf
#undef agent_init

#define GEN_MOCK_OBJECT( x )  mock_functions x; mock_ptr = &x;
#define GEN_NICEMOCK_OBJECT( x )  NiceMock<mock_functions> x; mock_ptr = &x;

using namespace ::testing;

// Static Functions Testing
// Test agent_cleanup
class Agent_Cleanup_Test : public ::testing::TestWithParam< int > { };

TEST_P ( Agent_Cleanup_Test, AGENT_CLEANUP ) {
    GEN_MOCK_OBJECT( mock );
    // while( pthread_mutex_trylock(&ac->thread.mtx) == EBUSY )
    EXPECT_CALL( mock, pthread_cancel(_) );
    EXPECT_CALL( mock, pthread_join(_,_) );
    EXPECT_CALL( mock, pthread_mutex_trylock(_) ).WillOnce( Return(0) );
    if ( GetParam() != 0 )
        EXPECT_CALL( mock, pthread_mutex_trylock(_) ).Times(GetParam()).WillRepeatedly( Return(EBUSY) ).RetiresOnSaturation();
    EXPECT_CALL( mock, pthread_mutex_unlock(_) ).Times(GetParam()+1);
    EXPECT_CALL( mock, pthread_mutex_destroy(_) ).Times(2);
    EXPECT_CALL( mock, pthread_mutex_trylock(_) ).WillOnce( Return(0) ).RetiresOnSaturation();
    if ( GetParam() != 0 )
        EXPECT_CALL( mock, pthread_mutex_trylock(_) ).Times(GetParam()).WillRepeatedly( Return(EBUSY) ).RetiresOnSaturation();
    EXPECT_CALL( mock, pthread_mutex_unlock(_) ).Times(GetParam()+1).RetiresOnSaturation();

    _agent_cleanup();
}

INSTANTIATE_TEST_CASE_P(Agent_Cleanup_Trylock_Times, Agent_Cleanup_Test, ::testing::Values(0, 1, 1000) ); 

// Test as_write
TEST ( AgentUnitTest, WRITE_FAIL ) {
    GEN_MOCK_OBJECT( mock );
    
    EXPECT_CALL( mock, sendto(_,_,_,_,_,_) ).WillOnce( Return( -1 ) );
    EXPECT_CALL( mock, agent_printf(_));
    EXPECT_CALL( mock, agent_cleanup());
    ASSERT_EQ( _as_write(123,NULL,1111), -1 );
}

// Test as_read
TEST ( AgentUnitTest, READ_FAIL ) {
    GEN_MOCK_OBJECT( mock );
    
    EXPECT_CALL( mock, recvfrom(_,_,_,_,_,_)).WillOnce( Return( 0 ) );
    EXPECT_CALL( mock, agent_printf(_));
    EXPECT_CALL( mock, agent_cleanup());
    ASSERT_EQ( _as_read(123,NULL,1111), 0 );
}

ACTION_P ( COMPARE, n ) { EXPECT_STREQ((char*)arg1, n); }
// Test ecm_write
TEST ( AgentUnitTest, ECM_WRITE ) {
    GEN_MOCK_OBJECT( mock );
    // Send MSG_REC_SUCCESS 
    EXPECT_CALL( mock, as_write(_,_,_) ).WillOnce( DoAll( COMPARE(MSG_REC_SUCCESS), Return(sizeof(MSG_REC_SUCCESS) ) ) ) ;
    _ecm_write(TRUE);
    // Send MSG_REC_FAIL
    EXPECT_CALL( mock, as_write(_,_,_) ).WillOnce( DoAll( COMPARE(MSG_REC_FAIL), Return(sizeof(MSG_REC_FAIL) ) ) ) ;
    _ecm_write(FALSE);
}

// Test ecm_read
TEST ( AgentUnitTest, ECM_READ ) {
    GEN_MOCK_OBJECT( mock );
    
    EXPECT_CALL( mock, as_read(_,_,_)).WillOnce( Return( 0 ) );
    ASSERT_EQ( _ecm_read(), FALSE );

    EXPECT_CALL( mock, as_read(_,_,_)).WillOnce( Return( sizeof(MSG_REC_SUCCESS) ) );
    EXPECT_CALL( mock, strncmp(_,_,_)).WillOnce( Return( 10 ) );
    ASSERT_EQ( _ecm_read(), FALSE );

    EXPECT_CALL( mock, as_read(_,_,_)).WillOnce( Return( sizeof(MSG_REC_SUCCESS) ) );
    EXPECT_CALL( mock, strncmp(_,_,_)).WillOnce( Return( 0 ) );
    ASSERT_EQ( _ecm_read(), TRUE );
}

class Set_Agent_Action_Test : public ::testing::TestWithParam< enum MBA_AGENT_ACTION > { };

TEST_P ( Set_Agent_Action_Test, SET_AGENT_ACTION ) {
    GEN_MOCK_OBJECT( mock );
    ac->act.type = AGENT_ACT_IDLE;
    EXPECT_CALL( mock, pthread_mutex_lock(_));
    EXPECT_CALL( mock, pthread_mutex_unlock(_));
    ASSERT_EQ( _set_agent_action(GetParam()), AGENT_RET_SUCCESS );
    ASSERT_EQ( ac->act.type, GetParam() );
}

INSTANTIATE_TEST_CASE_P(InstantiationName, Set_Agent_Action_Test, 
                        ::testing::Values(AGENT_ACT_IDLE, AGENT_ACT_INIT, 
                                          AGENT_ACT_IMPO, AGENT_ACT_EXPO, 
                                          AGENT_ACT_EXEC, AGENT_ACT_INVO, 
                                          AGENT_ACT_LOGF, AGENT_ACT_SYNC)
                       );

class Set_Agent_Action_Race_Condition_Test : public ::testing::TestWithParam< enum MBA_AGENT_ACTION > { };

TEST_P ( Set_Agent_Action_Race_Condition_Test, SET_AGENT_ACTION ) {
    GEN_MOCK_OBJECT( mock );
    // From action to idle mode
    ac->act.type = GetParam();
    EXPECT_CALL( mock, pthread_mutex_lock(_)).WillOnce( Return(0) );
    EXPECT_CALL( mock, pthread_mutex_unlock(_)).WillOnce( Return(0) );
    ASSERT_EQ( _set_agent_action(AGENT_ACT_IDLE), AGENT_RET_SUCCESS );
    ASSERT_EQ( ac->act.type, AGENT_ACT_IDLE );
    // Fail other action setting if current action has not finished
    // IMPO
    ac->act.type = GetParam();
    EXPECT_CALL( mock, pthread_mutex_lock(_)).Times(2).WillRepeatedly( Return(0) );
    EXPECT_CALL( mock, pthread_mutex_unlock(_)).Times(2).WillOnce( Return(0) ).WillOnce( Return(1) );
    ASSERT_EQ( _set_agent_action(AGENT_ACT_IMPO), AGENT_RET_EBUSY );
    ASSERT_EQ( ac->act.type, GetParam() );
    ASSERT_EQ( _set_agent_action(AGENT_ACT_IMPO), AGENT_RET_EFAIL );
    ASSERT_EQ( ac->act.type, GetParam() );
    // EXPO
    ac->act.type = GetParam();
    EXPECT_CALL( mock, pthread_mutex_lock(_)).Times(2).WillRepeatedly( Return(0) );
    EXPECT_CALL( mock, pthread_mutex_unlock(_)).Times(2).WillOnce( Return(0) ).WillOnce( Return(1) );
    ASSERT_EQ( _set_agent_action(AGENT_ACT_EXPO), AGENT_RET_EBUSY );
    ASSERT_EQ( ac->act.type, GetParam() );
    ASSERT_EQ( _set_agent_action(AGENT_ACT_EXPO), AGENT_RET_EFAIL );
    ASSERT_EQ( ac->act.type, GetParam() );
    // EXEC
    ac->act.type = GetParam();
    EXPECT_CALL( mock, pthread_mutex_lock(_)).Times(2).WillRepeatedly( Return(0) );
    EXPECT_CALL( mock, pthread_mutex_unlock(_)).Times(2).WillOnce( Return(0) ).WillOnce( Return(1) );
    ASSERT_EQ( _set_agent_action(AGENT_ACT_EXEC), AGENT_RET_EBUSY );
    ASSERT_EQ( ac->act.type, GetParam() );
    ASSERT_EQ( _set_agent_action(AGENT_ACT_EXEC), AGENT_RET_EFAIL );
    ASSERT_EQ( ac->act.type, GetParam() );
    // INVO
    ac->act.type = GetParam();
    EXPECT_CALL( mock, pthread_mutex_lock(_)).Times(2).WillRepeatedly( Return(0) );
    EXPECT_CALL( mock, pthread_mutex_unlock(_)).Times(2).WillOnce( Return(0) ).WillOnce( Return(1) );
    ASSERT_EQ( _set_agent_action(AGENT_ACT_INVO), AGENT_RET_EBUSY );
    ASSERT_EQ( ac->act.type, GetParam() );
    ASSERT_EQ( _set_agent_action(AGENT_ACT_INVO), AGENT_RET_EFAIL );
    ASSERT_EQ( ac->act.type, GetParam() );
    // LOGF
    ac->act.type = GetParam();
    EXPECT_CALL( mock, pthread_mutex_lock(_)).Times(2).WillRepeatedly( Return(0) );
    EXPECT_CALL( mock, pthread_mutex_unlock(_)).Times(2).WillOnce( Return(0) ).WillOnce( Return(1) );
    ASSERT_EQ( _set_agent_action(AGENT_ACT_LOGF), AGENT_RET_EBUSY );
    ASSERT_EQ( ac->act.type, GetParam() );
    ASSERT_EQ( _set_agent_action(AGENT_ACT_LOGF), AGENT_RET_EFAIL );
    ASSERT_EQ( ac->act.type, GetParam() );
    // SYNC
    ac->act.type = GetParam();
    EXPECT_CALL( mock, pthread_mutex_lock(_)).Times(2).WillRepeatedly( Return(0) );
    EXPECT_CALL( mock, pthread_mutex_unlock(_)).Times(2).WillOnce( Return(0) ).WillOnce( Return(1) );
    ASSERT_EQ( _set_agent_action(AGENT_ACT_SYNC), AGENT_RET_EBUSY );
    ASSERT_EQ( ac->act.type, GetParam() );
    ASSERT_EQ( _set_agent_action(AGENT_ACT_SYNC), AGENT_RET_EFAIL );
    ASSERT_EQ( ac->act.type, GetParam() );
    // INIT
    ac->act.type = GetParam();
    EXPECT_CALL( mock, pthread_mutex_lock(_)).Times(2).WillRepeatedly( Return(0) );
    EXPECT_CALL( mock, pthread_mutex_unlock(_)).Times(2).WillOnce( Return(0) ).WillOnce( Return(1) );
    ASSERT_EQ( _set_agent_action(AGENT_ACT_INIT), AGENT_RET_EBUSY );
    ASSERT_EQ( ac->act.type, GetParam() );
    ASSERT_EQ( _set_agent_action(AGENT_ACT_INIT), AGENT_RET_EFAIL );
    ASSERT_EQ( ac->act.type, GetParam() );
}

INSTANTIATE_TEST_CASE_P(InstantiationName, Set_Agent_Action_Race_Condition_Test, 
                        ::testing::Values(AGENT_ACT_INIT, 
                                          AGENT_ACT_IMPO, AGENT_ACT_EXPO, 
                                          AGENT_ACT_EXEC, AGENT_ACT_INVO, 
                                          AGENT_ACT_LOGF, AGENT_ACT_SYNC)
                       );

ACTION_P( STAT, n ) { arg1->st_size = n; }
// Test import_host_file
TEST ( AgentUnitTest, IMPORT_HOST_FILE_STANDARD_FUNCTIONS ) {
    GEN_MOCK_OBJECT( mock );
    // open file
    EXPECT_CALL( mock, open(_) ).WillOnce( Return( -1 ) );
    EXPECT_CALL( mock, agent_printf(_));
    ASSERT_EQ(_import_host_file(), AGENT_RET_EFAIL);
    // stat
    EXPECT_CALL( mock, open(_) ).WillOnce( Return( 10 ) );
    EXPECT_CALL( mock, stat(_,_)).WillOnce( Return( -1 ) );
    EXPECT_CALL( mock, agent_printf(_));
    EXPECT_CALL( mock, close(10) );
    ASSERT_EQ(_import_host_file(), AGENT_RET_EFAIL);
    // read file
    EXPECT_CALL( mock, open(_) ).WillOnce( Return( 20 ) );
    EXPECT_CALL( mock, stat(_,_)).WillOnce( DoAll(STAT(256), Return( 0 ) ) );
    EXPECT_CALL( mock, as_write(_,_,_) ).WillOnce( Return( SZ_MAX_COMMAND ) ).WillOnce( Return( sizeof(uint64_t) ) );
    EXPECT_CALL( mock, ecm_read() ).WillOnce( Return( TRUE ) );
    EXPECT_CALL( mock, read(_,_,_) ).WillOnce( Return( -1 ) );
    EXPECT_CALL( mock, agent_printf(_));
    EXPECT_CALL( mock, ecm_write( FALSE ) );
    EXPECT_CALL( mock, close(20) );
    ASSERT_EQ(_import_host_file(), AGENT_RET_EFAIL);
    // complete
    EXPECT_CALL( mock, open(_) ).WillOnce( Return( 30 ) );
    EXPECT_CALL( mock, stat(_,_)).WillOnce( DoAll(STAT(256), Return( 0 ) ) );
    EXPECT_CALL( mock, as_write(_,_,_) ).WillOnce( Return( SZ_MAX_COMMAND ) ).WillOnce( Return( sizeof(uint64_t) ) ).WillOnce( Return( 256 ) );
    EXPECT_CALL( mock, ecm_read() ).WillOnce( Return( TRUE ) ).WillOnce( Return( TRUE ) ).WillOnce( Return( TRUE ) );
    EXPECT_CALL( mock, read(_,_,_) ).WillOnce( Return( 256 ) );
    EXPECT_CALL( mock, ecm_write( TRUE ) );
    EXPECT_CALL( mock, close(30) );
    ASSERT_EQ(_import_host_file(), AGENT_RET_SUCCESS);
}
TEST ( AgentUnitTest, IMPORT_HOST_FILE_STATIC_FUNCTIONS ) {
    GEN_MOCK_OBJECT( mock );
    // emit command 
    EXPECT_CALL( mock, open(_) ).WillOnce( Return( 10 ) );
    EXPECT_CALL( mock, stat(_,_)).WillOnce( Return( 0 ) );
    EXPECT_CALL( mock, as_write(_,_,_) ).WillOnce( Return(0) );
    EXPECT_CALL( mock, agent_printf(_));
    EXPECT_CALL( mock, close(10) );
    ASSERT_EQ(_import_host_file(), AGENT_RET_EFAIL);
    // send file size
    EXPECT_CALL( mock, open(_) ).WillOnce( Return( 20 ) );
    EXPECT_CALL( mock, stat(_,_)).WillOnce( Return( 0 ) );
    EXPECT_CALL( mock, as_write(_,_,_) ).WillOnce( Return( SZ_MAX_COMMAND ) ).WillOnce( Return( 0 ) );
    EXPECT_CALL( mock, agent_printf(_));
    EXPECT_CALL( mock, close(20) );
    ASSERT_EQ(_import_host_file(), AGENT_RET_EFAIL);
    // error check 
    EXPECT_CALL( mock, open(_) ).WillOnce( Return( 30 ) );
    EXPECT_CALL( mock, stat(_,_)).WillOnce( Return( 0 ) );
    EXPECT_CALL( mock, as_write(_,_,_) ).WillOnce( Return( SZ_MAX_COMMAND ) ).WillOnce( Return( sizeof(uint64_t) ) );
    EXPECT_CALL( mock, ecm_read()).WillOnce( Return( FALSE ) );
    EXPECT_CALL( mock, agent_printf(_));
    EXPECT_CALL( mock, close(30) );
    ASSERT_EQ(_import_host_file(), AGENT_RET_EFAIL);
    // read file content 
    EXPECT_CALL( mock, open(_) ).WillOnce( Return( 40 ) );
    EXPECT_CALL( mock, stat(_,_)).WillOnce( DoAll(STAT(256), Return( 0 ) ) );
    EXPECT_CALL( mock, as_write(_,_,_) ).WillOnce( Return( SZ_MAX_COMMAND ) ).WillOnce( Return( sizeof(uint64_t) ) );
    EXPECT_CALL( mock, ecm_read() ).WillOnce( Return( TRUE ) );
    EXPECT_CALL( mock, read(_,_,_) ).WillOnce( Return( -1 ) );
    EXPECT_CALL( mock, ecm_write( FALSE ) );
    EXPECT_CALL( mock, agent_printf(_));
    EXPECT_CALL( mock, close(40) );
    ASSERT_EQ(_import_host_file(), AGENT_RET_EFAIL);
    // send file content
    EXPECT_CALL( mock, open(_) ).WillOnce( Return( 50 ) );
    EXPECT_CALL( mock, stat(_,_)).WillOnce( DoAll(STAT(256), Return( 0 ) ) );
    EXPECT_CALL( mock, as_write(_,_,_) ).WillOnce( Return( SZ_MAX_COMMAND ) ).WillOnce( Return( sizeof(uint64_t) ) ).WillOnce( Return( 0 ) );
    EXPECT_CALL( mock, ecm_read() ).WillOnce( Return( TRUE ) );
    EXPECT_CALL( mock, read(_,_,_) ).WillOnce( Return( 128 ) );
    EXPECT_CALL( mock, ecm_write( TRUE ) );
    EXPECT_CALL( mock, agent_printf(_));
    EXPECT_CALL( mock, close(50) );
    ASSERT_EQ(_import_host_file(), AGENT_RET_EFAIL);
    // error check 
    EXPECT_CALL( mock, open(_) ).WillOnce( Return( 60 ) );
    EXPECT_CALL( mock, stat(_,_)).WillOnce( DoAll(STAT(256), Return( 0 ) ) );
    EXPECT_CALL( mock, as_write(_,_,_) ).WillOnce( Return( SZ_MAX_COMMAND ) ).WillOnce( Return( sizeof(uint64_t) ) ).WillOnce( Return( 128 ) );
    EXPECT_CALL( mock, ecm_read() ).WillOnce( Return( TRUE ) ).WillOnce( Return( FALSE ) );
    EXPECT_CALL( mock, read(_,_,_) ).WillOnce( Return( 128 ) );
    EXPECT_CALL( mock, ecm_write( TRUE ) );
    EXPECT_CALL( mock, agent_printf(_));
    EXPECT_CALL( mock, close(60) );
    ASSERT_EQ(_import_host_file(), AGENT_RET_EFAIL);
    // error check, less than SZ_MAX_FILECHUNK
    EXPECT_CALL( mock, open(_) ).WillOnce( Return( 70 ) );
    EXPECT_CALL( mock, stat(_,_)).WillOnce( DoAll(STAT(256), Return( 0 ) ) );
    EXPECT_CALL( mock, as_write(_,_,_) ).WillOnce( Return( SZ_MAX_COMMAND ) ).WillOnce( Return( sizeof(uint64_t) ) ).WillOnce( Return( 128 ) );
    EXPECT_CALL( mock, ecm_read() ).WillOnce( Return( TRUE ) ).WillOnce( Return( TRUE ) ).WillOnce( Return( FALSE ) );
    EXPECT_CALL( mock, read(_,_,_) ).WillOnce( Return( 128 ) );
    EXPECT_CALL( mock, ecm_write( TRUE ) );
    EXPECT_CALL( mock, agent_printf(_));
    EXPECT_CALL( mock, close(70) );
    ASSERT_EQ(_import_host_file(), AGENT_RET_EFAIL);
    // complete 
    EXPECT_CALL( mock, open(_) ).WillOnce( Return( 80 ) );
    EXPECT_CALL( mock, stat(_,_)).WillOnce( DoAll(STAT(256), Return( 0 ) ) );
    EXPECT_CALL( mock, as_write(_,_,_) ).WillOnce( Return( SZ_MAX_COMMAND ) ).WillOnce( Return( sizeof(uint64_t) ) ).WillOnce( Return( 256 ) );
    EXPECT_CALL( mock, ecm_read() ).WillOnce( Return( TRUE ) ).WillOnce( Return( TRUE ) ).WillOnce( Return( TRUE ) );
    EXPECT_CALL( mock, read(_,_,_) ).WillOnce( Return( 256 ) );
    EXPECT_CALL( mock, ecm_write( TRUE ) );
    EXPECT_CALL( mock, close(80) );
    ASSERT_EQ(_import_host_file(), AGENT_RET_SUCCESS);
}

ACTION_P( GETSZ, n ) { *(uint64_t*)arg1 = n; }
// Test export_guest_file
TEST ( AgentUnitTest, EXPORT_GUEST_FILE_STANDARD_FUNCTIONS ) {
    GEN_MOCK_OBJECT( mock );
    // open file
    EXPECT_CALL( mock, as_write(_,_,_) ).WillOnce( Return( SZ_MAX_COMMAND ) );
    EXPECT_CALL( mock, ecm_read() ).WillOnce( Return( TRUE ) );
    EXPECT_CALL( mock, as_read(_,_,_) ).WillOnce( Return( sizeof(uint64_t) ) );
    EXPECT_CALL( mock, open(_) ).WillOnce( Return( -1 ) );
    EXPECT_CALL( mock, agent_printf(_));
    EXPECT_CALL( mock, ecm_write( FALSE ) );
    ASSERT_EQ(_export_guest_file(), AGENT_RET_EFAIL);
    // write file
    EXPECT_CALL( mock, as_write(_,_,_) ).WillOnce( Return( SZ_MAX_COMMAND ) );
    EXPECT_CALL( mock, ecm_read()).WillOnce( Return( TRUE ) ).WillOnce( Return( TRUE ) );
    EXPECT_CALL( mock, as_read(_,_,_) ).WillOnce( DoAll( GETSZ(256), Return( sizeof(uint64_t) ) ) ).WillOnce( Return(256) ) ;
    EXPECT_CALL( mock, open(_) ).WillOnce( Return( 10 ) );
    EXPECT_CALL( mock, ecm_write( TRUE ) );
    EXPECT_CALL( mock, write(_,_,_)).WillOnce( Return( -1 ) );
    EXPECT_CALL( mock, agent_printf(_));
    EXPECT_CALL( mock, ecm_write( FALSE ) );
    EXPECT_CALL( mock, close(10) );
    ASSERT_EQ(_export_guest_file(), AGENT_RET_EFAIL);
    // complete 
    EXPECT_CALL( mock, as_write(_,_,_) ).WillOnce( Return( SZ_MAX_COMMAND ) );
    EXPECT_CALL( mock, ecm_read()).WillOnce( Return( TRUE ) ).WillOnce( Return( TRUE ) );
    EXPECT_CALL( mock, as_read(_,_,_) ).WillOnce( DoAll( GETSZ(256), Return( sizeof(uint64_t) ) ) ).WillOnce( Return(256) ) ;
    EXPECT_CALL( mock, open(_) ).WillOnce( Return( 20 ) );
    EXPECT_CALL( mock, ecm_write( TRUE ) ).Times(2);
    EXPECT_CALL( mock, write(_,_,_)).WillOnce( Return( 256 ) );
    EXPECT_CALL( mock, close(20) );
    ASSERT_EQ(_export_guest_file(), AGENT_RET_SUCCESS);
}
TEST ( AgentUnitTest, EXPORT_GUEST_FILE_STATIC_FUNCTIONS ) {
    GEN_MOCK_OBJECT( mock );
    // emit command
    EXPECT_CALL( mock, as_write(_,_,_) ).WillOnce( Return( 0 ) );
    EXPECT_CALL( mock, agent_printf(_));
    ASSERT_EQ(_export_guest_file(), AGENT_RET_EFAIL);
    // error check
    EXPECT_CALL( mock, as_write(_,_,_) ).WillOnce( Return( SZ_MAX_COMMAND ) );
    EXPECT_CALL( mock, ecm_read() ).WillOnce( Return( FALSE ) );
    EXPECT_CALL( mock, agent_printf(_));
    ASSERT_EQ(_export_guest_file(), AGENT_RET_EFAIL);
    // receive file size
    EXPECT_CALL( mock, as_write(_,_,_) ).WillOnce( Return( SZ_MAX_COMMAND ) );
    EXPECT_CALL( mock, ecm_read() ).WillOnce( Return( TRUE ) );
    EXPECT_CALL( mock, as_read(_,_,_) ).WillOnce( Return( 0 ) );
    EXPECT_CALL( mock, agent_printf(_));
    ASSERT_EQ(_export_guest_file(), AGENT_RET_EFAIL);
    // error check
    EXPECT_CALL( mock, as_write(_,_,_) ).WillOnce( Return( SZ_MAX_COMMAND ) );
    EXPECT_CALL( mock, ecm_read() ).WillOnce( Return( TRUE ) ).WillOnce( Return( FALSE ) );
    EXPECT_CALL( mock, as_read(_,_,_) ).WillOnce( DoAll( GETSZ(256), Return( sizeof(uint64_t) ) ) );
    EXPECT_CALL( mock, open(_) ).WillOnce( Return( 10 ) );
    EXPECT_CALL( mock, ecm_write( TRUE ) );
    EXPECT_CALL( mock, agent_printf(_));
    EXPECT_CALL( mock, close(10));
    ASSERT_EQ(_export_guest_file(), AGENT_RET_EFAIL);
    // receive file content
    EXPECT_CALL( mock, as_write(_,_,_) ).WillOnce( Return( SZ_MAX_COMMAND ) );
    EXPECT_CALL( mock, ecm_read() ).WillOnce( Return( TRUE ) ).WillOnce( Return( TRUE ) );
    EXPECT_CALL( mock, as_read(_,_,_) ).WillOnce( DoAll( GETSZ(256), Return( sizeof(uint64_t) ) ) ).WillOnce( Return( 0 ) );
    EXPECT_CALL( mock, open(_) ).WillOnce( Return( 20 ) );
    EXPECT_CALL( mock, ecm_write( TRUE ) );
    EXPECT_CALL( mock, agent_printf(_));
    EXPECT_CALL( mock, close(20));
    ASSERT_EQ(_export_guest_file(), AGENT_RET_EFAIL);
    // complete
    EXPECT_CALL( mock, as_write(_,_,_) ).WillOnce( Return( SZ_MAX_COMMAND ) );
    EXPECT_CALL( mock, ecm_read() ).WillOnce( Return( TRUE ) ).WillOnce( Return( TRUE ) );
    EXPECT_CALL( mock, as_read(_,_,_) ).WillOnce( DoAll( GETSZ(256), Return( sizeof(uint64_t) ) ) ).WillOnce( Return( 256 ) );
    EXPECT_CALL( mock, open(_) ).WillOnce( Return( 30 ) );
    EXPECT_CALL( mock, ecm_write( TRUE ) ).Times(2);
    EXPECT_CALL( mock, write(_,_,_) ).WillOnce( Return(256) );
    EXPECT_CALL( mock, close(30));
    ASSERT_EQ(_export_guest_file(), AGENT_RET_SUCCESS);
}

// Test execute_guest_cmd_return
TEST ( AgentUnitTest, EXECUTE_GUEST_CMD_RETURN_STATIC_FUNCTIONS ) {
    GEN_MOCK_OBJECT( mock );
    // emit command
    EXPECT_CALL( mock, as_write(_,_,_) ).WillOnce( Return( 0 ) );
    EXPECT_CALL( mock, agent_printf(_) );
    ASSERT_EQ( _execute_guest_cmd_return(), AGENT_RET_EFAIL );
    // error check
    EXPECT_CALL( mock, as_write(_,_,_) ).WillOnce( Return( SZ_MAX_COMMAND ) );
    EXPECT_CALL( mock, ecm_read() ).WillOnce( Return( FALSE ) );
    EXPECT_CALL( mock, agent_printf(_) );
    ASSERT_EQ( _execute_guest_cmd_return(), AGENT_RET_EFAIL );
    // get output message size
    EXPECT_CALL( mock, as_write(_,_,_) ).WillOnce( Return( SZ_MAX_COMMAND ) );
    EXPECT_CALL( mock, ecm_read() ).WillOnce( Return( TRUE ) );
    EXPECT_CALL( mock, as_read(_,_,_) ).WillOnce( Return( 0 ) );
    ASSERT_EQ( _execute_guest_cmd_return(), AGENT_RET_EFAIL );
    // read output message
    EXPECT_CALL( mock, as_write(_,_,_) ).WillOnce( Return( SZ_MAX_COMMAND ) );
    EXPECT_CALL( mock, ecm_read() ).WillOnce( Return( TRUE ) );
    EXPECT_CALL( mock, as_read(_,_,_) ).WillOnce( DoAll( GETSZ(256), Return( sizeof(uint32_t) ) ) ).WillOnce( Return( 0 ) );
    EXPECT_CALL( mock, agent_printf(_));
    ASSERT_EQ( _execute_guest_cmd_return(), AGENT_RET_EFAIL );
    // error_check
    EXPECT_CALL( mock, as_write(_,_,_) ).WillOnce( Return( SZ_MAX_COMMAND ) );
    EXPECT_CALL( mock, ecm_read() ).WillOnce( Return( TRUE ) ).WillOnce( Return( FALSE ) );
    EXPECT_CALL( mock, as_read(_,_,_) ).WillOnce( DoAll( GETSZ(256), Return( sizeof(uint32_t) ) ) ).WillOnce( Return( 256 ) ).WillOnce( DoAll( GETSZ(0), Return( sizeof(uint32_t) ) ) );
    EXPECT_CALL( mock, agent_printf(_)).Times(257);
    ASSERT_EQ( _execute_guest_cmd_return(), AGENT_RET_EFAIL );
    // complete 
    EXPECT_CALL( mock, as_write(_,_,_) ).WillOnce( Return( SZ_MAX_COMMAND ) );
    EXPECT_CALL( mock, ecm_read() ).WillOnce( Return( TRUE ) ).WillOnce( Return( TRUE ) );
    EXPECT_CALL( mock, as_read(_,_,_) ).WillOnce( DoAll( GETSZ(256), Return( sizeof(uint32_t) ) ) ).WillOnce( Return( 256 ) ).WillOnce( DoAll( GETSZ(0), Return( sizeof(uint32_t) ) ));
    EXPECT_CALL( mock, agent_printf(_)).Times(256);
    ASSERT_EQ( _execute_guest_cmd_return(), AGENT_RET_SUCCESS );
}

// Test execute_guest_cmd_noreturn
TEST ( AgentUnitTest, EXECUTE_GUEST_CMD_NORETURN_STATIC_FUNCTIONS ) {
    GEN_MOCK_OBJECT( mock );
    // emit command
    EXPECT_CALL( mock, as_write(_,_,_) ).WillOnce( Return( 0 ) );
    EXPECT_CALL( mock, agent_printf(_) );
    ASSERT_EQ( _execute_guest_cmd_noreturn(), AGENT_RET_EFAIL );
    // error check
    EXPECT_CALL( mock, as_write(_,_,_) ).WillOnce( Return( SZ_MAX_COMMAND ) );
    EXPECT_CALL( mock, ecm_read() ).WillOnce( Return( FALSE ) );
    EXPECT_CALL( mock, agent_printf(_) );
    ASSERT_EQ( _execute_guest_cmd_noreturn(), AGENT_RET_EFAIL );
    // complete 
    EXPECT_CALL( mock, as_write(_,_,_) ).WillOnce( Return( SZ_MAX_COMMAND ) );
    EXPECT_CALL( mock, ecm_read() ).WillOnce( Return( TRUE ) );
    ASSERT_EQ( _execute_guest_cmd_noreturn(), AGENT_RET_SUCCESS );
}

// Test export_agent_log
TEST ( AgentUnitTest, EXPORT_AGENT_LOG_STANDARD_FUNCTIONS ) {
    GEN_MOCK_OBJECT( mock );
    // open file
    EXPECT_CALL( mock, as_write(_,_,_) ).WillOnce( Return( SZ_MAX_COMMAND ) );
    EXPECT_CALL( mock, ecm_read() ).WillOnce( Return( TRUE ) );
    EXPECT_CALL( mock, as_read(_,_,_) ).WillOnce( Return( sizeof(uint64_t) ) );
    EXPECT_CALL( mock, open(_) ).WillOnce( Return( -1 ) );
    EXPECT_CALL( mock, agent_printf(_));
    EXPECT_CALL( mock, ecm_write( FALSE ) );
    ASSERT_EQ(_export_agent_log(), AGENT_RET_EFAIL);
    // write file
    EXPECT_CALL( mock, as_write(_,_,_) ).WillOnce( Return( SZ_MAX_COMMAND ) );
    EXPECT_CALL( mock, ecm_read()).WillOnce( Return( TRUE ) ).WillOnce( Return( TRUE ) );
    EXPECT_CALL( mock, as_read(_,_,_) ).WillOnce( DoAll( GETSZ(256), Return( sizeof(uint64_t) ) ) ).WillOnce( Return(256) ) ;
    EXPECT_CALL( mock, open(_) ).WillOnce( Return( 10 ) );
    EXPECT_CALL( mock, ecm_write( TRUE ) );
    EXPECT_CALL( mock, write(_,_,_)).WillOnce( Return( -1 ) );
    EXPECT_CALL( mock, agent_printf(_));
    EXPECT_CALL( mock, ecm_write( FALSE ) );
    EXPECT_CALL( mock, close(10) );
    ASSERT_EQ(_export_agent_log(), AGENT_RET_EFAIL);
    // complete 
    EXPECT_CALL( mock, as_write(_,_,_) ).WillOnce( Return( SZ_MAX_COMMAND ) );
    EXPECT_CALL( mock, ecm_read()).WillOnce( Return( TRUE ) ).WillOnce( Return( TRUE ) );
    EXPECT_CALL( mock, as_read(_,_,_) ).WillOnce( DoAll( GETSZ(256), Return( sizeof(uint64_t) ) ) ).WillOnce( Return(256) ) ;
    EXPECT_CALL( mock, open(_) ).WillOnce( Return( 20 ) );
    EXPECT_CALL( mock, ecm_write( TRUE ) ).Times(2);
    EXPECT_CALL( mock, write(_,_,_)).WillOnce( Return( 256 ) );
    EXPECT_CALL( mock, close(20) );
    ASSERT_EQ(_export_agent_log(), AGENT_RET_SUCCESS);
}
TEST ( AgentUnitTest, EXPORT_AGENT_LOG_STATIC_FUNCTIONS ) {
    GEN_MOCK_OBJECT( mock );
    // emit command
    EXPECT_CALL( mock, as_write(_,_,_) ).WillOnce( Return( 0 ) );
    EXPECT_CALL( mock, agent_printf(_));
    ASSERT_EQ(_export_agent_log(), AGENT_RET_EFAIL);
    // error check
    EXPECT_CALL( mock, as_write(_,_,_) ).WillOnce( Return( SZ_MAX_COMMAND ) );
    EXPECT_CALL( mock, ecm_read() ).WillOnce( Return( FALSE ) );
    EXPECT_CALL( mock, agent_printf(_));
    ASSERT_EQ(_export_agent_log(), AGENT_RET_EFAIL);
    // receive file size
    EXPECT_CALL( mock, as_write(_,_,_) ).WillOnce( Return( SZ_MAX_COMMAND ) );
    EXPECT_CALL( mock, ecm_read() ).WillOnce( Return( TRUE ) );
    EXPECT_CALL( mock, as_read(_,_,_) ).WillOnce( Return( 0 ) );
    EXPECT_CALL( mock, agent_printf(_));
    ASSERT_EQ(_export_agent_log(), AGENT_RET_EFAIL);
    // error check
    EXPECT_CALL( mock, as_write(_,_,_) ).WillOnce( Return( SZ_MAX_COMMAND ) );
    EXPECT_CALL( mock, ecm_read() ).WillOnce( Return( TRUE ) ).WillOnce( Return( FALSE ) );
    EXPECT_CALL( mock, as_read(_,_,_) ).WillOnce( DoAll( GETSZ(256), Return( sizeof(uint64_t) ) ) );
    EXPECT_CALL( mock, open(_) ).WillOnce( Return( 10 ) );
    EXPECT_CALL( mock, ecm_write( TRUE ) );
    EXPECT_CALL( mock, agent_printf(_));
    EXPECT_CALL( mock, close(10));
    ASSERT_EQ(_export_agent_log(), AGENT_RET_EFAIL);
    // receive file content
    EXPECT_CALL( mock, as_write(_,_,_) ).WillOnce( Return( SZ_MAX_COMMAND ) );
    EXPECT_CALL( mock, ecm_read() ).WillOnce( Return( TRUE ) ).WillOnce( Return( TRUE ) );
    EXPECT_CALL( mock, as_read(_,_,_) ).WillOnce( DoAll( GETSZ(256), Return( sizeof(uint64_t) ) ) ).WillOnce( Return( 0 ) );
    EXPECT_CALL( mock, open(_) ).WillOnce( Return( 20 ) );
    EXPECT_CALL( mock, ecm_write( TRUE ) );
    EXPECT_CALL( mock, agent_printf(_));
    EXPECT_CALL( mock, close(20));
    ASSERT_EQ(_export_agent_log(), AGENT_RET_EFAIL);
    // complete
    EXPECT_CALL( mock, as_write(_,_,_) ).WillOnce( Return( SZ_MAX_COMMAND ) );
    EXPECT_CALL( mock, ecm_read() ).WillOnce( Return( TRUE ) ).WillOnce( Return( TRUE ) );
    EXPECT_CALL( mock, as_read(_,_,_) ).WillOnce( DoAll( GETSZ(256), Return( sizeof(uint64_t) ) ) ).WillOnce( Return( 256 ) );
    EXPECT_CALL( mock, open(_) ).WillOnce( Return( 30 ) );
    EXPECT_CALL( mock, ecm_write( TRUE ) ).Times(2);
    EXPECT_CALL( mock, write(_,_,_) ).WillOnce( Return(256) );
    EXPECT_CALL( mock, close(30));
    ASSERT_EQ(_export_agent_log(), AGENT_RET_SUCCESS);
}

// Test sync_cache
TEST ( AgentUnitTest, SYNC_CACHE_STATIC_FUNCTIONS ) {
    GEN_MOCK_OBJECT( mock );
    // emit command
    EXPECT_CALL( mock, as_write(_,_,_) ).WillOnce( Return( 0 ) );
    EXPECT_CALL( mock, agent_printf(_) );
    ASSERT_EQ( _sync_cache(), AGENT_RET_EFAIL );
    // error check
    EXPECT_CALL( mock, as_write(_,_,_) ).WillOnce( Return( SZ_MAX_COMMAND ) );
    EXPECT_CALL( mock, ecm_read() ).WillOnce( Return( FALSE ) );
    EXPECT_CALL( mock, agent_printf(_) );
    ASSERT_EQ( _sync_cache(), AGENT_RET_EFAIL );
    // complete
    EXPECT_CALL( mock, as_write(_,_,_) ).WillOnce( Return( SZ_MAX_COMMAND ) );
    EXPECT_CALL( mock, ecm_read() ).WillOnce( Return( TRUE ) );
    ASSERT_EQ( _sync_cache(), AGENT_RET_SUCCESS );
}

// Test show_server_ack
TEST ( AgentUnitTest, SHOW_SERVER_ACK_STATIC_FUNCTIONS ) {
    GEN_MOCK_OBJECT( mock );
    
    EXPECT_CALL( mock, as_read(_,_,_) ).WillOnce( Return( 0 ) ).WillOnce( Return( 0 ) );
    EXPECT_CALL( mock, agent_printf(_) ).Times(0);
    show_server_ack();
    
    EXPECT_CALL( mock, as_read(_,_,_) ).WillOnce( Return( sizeof(MSG_ACK_PREFIX) ) ).WillOnce( Return( 0 ) );
    EXPECT_CALL( mock, agent_printf(_) ).Times(1);
    show_server_ack();
    
    EXPECT_CALL( mock, as_read(_,_,_) ).WillOnce( Return( 0 ) ).WillOnce( Return( SZ_MAX_COMMAND ) );
    EXPECT_CALL( mock, agent_printf(_) ).Times(1);
    show_server_ack();
    
    EXPECT_CALL( mock, as_read(_,_,_) ).WillOnce( Return( sizeof(MSG_ACK_PREFIX) ) ).WillOnce( Return( SZ_MAX_COMMAND ) );
    EXPECT_CALL( mock, agent_printf(_) ).Times(2);
    show_server_ack();
}

// Test connect_agent_server 
TEST ( AgentUnitTest, CONNECT_AGENT_SERVER_STATIC_FUNCTIONS ) {
    GEN_MOCK_OBJECT( mock );
    // construct socket
    EXPECT_CALL( mock, socket(_,_,_) ).WillOnce( Return( -1 ) );
    ASSERT_EQ( _connect_agent_server(), -1 );
    // establish connection
    EXPECT_CALL( mock, socket(_,_,_) ).WillOnce( Return( 10 ) );
    EXPECT_CALL( mock, connect(10,_,_) ).WillOnce( Return( -1 ) );
    ASSERT_EQ( _connect_agent_server(), -1 );
    // complete
    EXPECT_CALL( mock, socket(_,_,_) ).WillOnce( Return( 20 ) );
    EXPECT_CALL( mock, connect(20,_,_) ).WillOnce( Return( 0 ) );
    ASSERT_EQ( _connect_agent_server(), 20 );
}


// Test Public API
// Test agent_is_ready
TEST ( AgentUnitTest, AGENT_IS_READY ) {
    ac->ready = TRUE;
    ASSERT_EQ( agent_is_ready(), TRUE );
    ac->ready = FALSE;
    ASSERT_EQ( agent_is_ready(), FALSE );
}

// Test agent_is_exec 
TEST ( AgentUnitTest, AGENT_IS_EXEC ) {
    GEN_MOCK_OBJECT( mock );
    // IDLE 
    EXPECT_CALL( mock, pthread_mutex_lock(_) );
    EXPECT_CALL( mock, pthread_mutex_unlock(_));
    ac->act.type = AGENT_ACT_EXEC;
    ASSERT_EQ( agent_is_exec(), TRUE );
    // INIT
    EXPECT_CALL( mock, pthread_mutex_lock(_));
    EXPECT_CALL( mock, pthread_mutex_unlock(_));
    ac->act.type = AGENT_ACT_INIT ;
    ASSERT_EQ( agent_is_exec(), FALSE );
    // IMPO
    EXPECT_CALL( mock, pthread_mutex_lock(_));
    EXPECT_CALL( mock, pthread_mutex_unlock(_));
    ac->act.type = AGENT_ACT_IMPO ;
    ASSERT_EQ( agent_is_exec(), FALSE );
    // EXPO
    EXPECT_CALL( mock, pthread_mutex_lock(_));
    EXPECT_CALL( mock, pthread_mutex_unlock(_));
    ac->act.type = AGENT_ACT_EXPO ;
    ASSERT_EQ( agent_is_exec(), FALSE );
    // EXEC
    EXPECT_CALL( mock, pthread_mutex_lock(_));
    EXPECT_CALL( mock, pthread_mutex_unlock(_));
    ac->act.type = AGENT_ACT_EXEC ;
    ASSERT_EQ( agent_is_exec(), TRUE );
    // INVO
    EXPECT_CALL( mock, pthread_mutex_lock(_));
    EXPECT_CALL( mock, pthread_mutex_unlock(_));
    ac->act.type = AGENT_ACT_INVO ;
    ASSERT_EQ( agent_is_exec(), FALSE );
    // LOGF
    EXPECT_CALL( mock, pthread_mutex_lock(_));
    EXPECT_CALL( mock, pthread_mutex_unlock(_));
    ac->act.type = AGENT_ACT_LOGF ;
    ASSERT_EQ( agent_is_exec(), FALSE );
    // SYNC
    EXPECT_CALL( mock, pthread_mutex_lock(_));
    EXPECT_CALL( mock, pthread_mutex_unlock(_));
    ac->act.type = AGENT_ACT_SYNC ;
    ASSERT_EQ( agent_is_exec(), FALSE );
}

// Test agent_printf
TEST ( AgentUnitTest, AGENT_PRINTF ) {
    GEN_MOCK_OBJECT( mock );
    EXPECT_CALL( mock, monitor_vprintf(_,_,_) ).Times(0);
    ac->mon = NULL;
    _agent_printf(NULL);
}

// Test agent_handle_exec_command
TEST ( AgentUnitTest, AGENT_HANDLE_EXEC_COMMAND ) {
    GEN_MOCK_OBJECT( mock );
    pthread_mutex_init(&ac->act.mtx, NULL);
    EXPECT_CALL( mock, pthread_mutex_lock(_)).Times(2);
    EXPECT_CALL( mock, pthread_mutex_unlock(_)).Times(2);
    
    ac->ready = FALSE;
    ac->act.type = AGENT_ACT_EXEC;
    EXPECT_CALL( mock, as_write(_,_,_) ).Times(0);
    agent_handle_exec_command("Agent is Good");

    ac->ready = FALSE;
    ac->act.type = AGENT_ACT_IDLE;
    EXPECT_CALL( mock, as_write(_,_,_) ).Times(0);
    agent_handle_exec_command("Agent is Good");

    ac->ready = TRUE;
    ac->act.type = AGENT_ACT_IDLE;
    EXPECT_CALL( mock, as_write(_,_,_) ).Times(0);
    agent_handle_exec_command("Agent is Good");

    ac->ready = TRUE;
    ac->act.type = AGENT_ACT_EXEC;
    EXPECT_CALL( mock, as_write(_,_,_) ).Times(1);
    agent_handle_exec_command("Agent is Good");
}

// Test agent_import
TEST ( AgentUnitTest, AGENT_IMPORT ) {
    GEN_MOCK_OBJECT( mock );

    EXPECT_CALL( mock, pthread_mutex_trylock(_) ).Times(6).WillOnce( Return( EBUSY ) ).WillRepeatedly( Return( 0 ) );
    EXPECT_CALL( mock, set_agent_action(AGENT_ACT_IMPO) ).Times(5).WillOnce( Return( AGENT_RET_EBUSY ) ).WillOnce( Return( AGENT_RET_EFAIL ) ).WillRepeatedly( Return( AGENT_RET_SUCCESS ) );
    EXPECT_CALL( mock, pthread_cond_signal(_) ).Times(3).WillOnce( Return( 1 ) ).WillRepeatedly( Return( 0 ) );
    EXPECT_CALL( mock, pthread_mutex_unlock(_) ).Times(2).WillOnce( Return( 1 ) ).WillRepeatedly( Return( 0 ) );

    // Check the agent is ready
    ac->ready = FALSE;
    ASSERT_EQ( agent_import(NULL, NULL), AGENT_RET_EINIT );
    // Check the agent is busy
    ac->ready = TRUE;
    ASSERT_EQ( agent_import(NULL, NULL), AGENT_RET_EBUSY );
    // Check set the right action
    ac->ready = TRUE;
    ASSERT_EQ( agent_import(NULL, NULL), AGENT_RET_EBUSY );
    ac->ready = TRUE;
    ASSERT_EQ( agent_import(NULL, NULL), AGENT_RET_EFAIL );
    // Check wake up agent main thread
    ac->ready = TRUE;
    ASSERT_EQ( agent_import("Agent is Good", "Agent is GOOD"), AGENT_RET_EFAIL );
    // Check release lock 
    ac->ready = TRUE;
    ASSERT_EQ( agent_import("Agent is GOOD", "Agent is GOOD"), AGENT_RET_EFAIL );
    // Complete
    ac->ready = TRUE;
    ASSERT_EQ( agent_import("Agent is GOOD", "Agent is GOOD"), AGENT_RET_SUCCESS );
}

// Test agent_export
TEST ( AgentUnitTest, AGENT_EXPORT ) {
    GEN_MOCK_OBJECT( mock );

    EXPECT_CALL( mock, pthread_mutex_trylock(_) ).Times(6).WillOnce( Return( EBUSY ) ).WillRepeatedly( Return( 0 ) );
    EXPECT_CALL( mock, set_agent_action(AGENT_ACT_EXPO) ).Times(5).WillOnce( Return( AGENT_RET_EBUSY ) ).WillOnce( Return( AGENT_RET_EFAIL ) ).WillRepeatedly( Return( AGENT_RET_SUCCESS ) );
    EXPECT_CALL( mock, pthread_cond_signal(_) ).Times(3).WillOnce( Return( 1 ) ).WillRepeatedly( Return( 0 ) );
    EXPECT_CALL( mock, pthread_mutex_unlock(_) ).Times(2).WillOnce( Return( 1 ) ).WillRepeatedly( Return( 0 ) );

    // Check the agent is ready
    ac->ready = FALSE;
    ASSERT_EQ( agent_export(NULL, NULL), AGENT_RET_EINIT );
    // Check the agent is busy
    ac->ready = TRUE;
    ASSERT_EQ( agent_export(NULL, NULL), AGENT_RET_EBUSY );
    // Check set the right action
    ac->ready = TRUE;
    ASSERT_EQ( agent_export(NULL, NULL), AGENT_RET_EBUSY );
    ac->ready = TRUE;
    ASSERT_EQ( agent_export(NULL, NULL), AGENT_RET_EFAIL );
    // Check wake up agent main thread
    ac->ready = TRUE;
    ASSERT_EQ( agent_export("Agent is Good", "Agent is GOOD"), AGENT_RET_EFAIL );
    // Check release lock 
    ac->ready = TRUE;
    ASSERT_EQ( agent_export("Agent is GOOD", "Agent is GOOD"), AGENT_RET_EFAIL );
    // Complete
    ac->ready = TRUE;
    ASSERT_EQ( agent_export("Agent is GOOD", "Agent is GOOD"), AGENT_RET_SUCCESS );

}

// Test agent_execute
TEST ( AgentUnitTest, AGENT_EXECUTE ) {
    GEN_MOCK_OBJECT( mock );

    EXPECT_CALL( mock, pthread_mutex_trylock(_) ).Times(6).WillOnce( Return( EBUSY ) ).WillRepeatedly( Return( 0 ) );
    EXPECT_CALL( mock, set_agent_action(AGENT_ACT_EXEC) ).Times(5).WillOnce( Return( AGENT_RET_EBUSY ) ).WillOnce( Return( AGENT_RET_EFAIL ) ).WillRepeatedly( Return( AGENT_RET_SUCCESS ) );
    EXPECT_CALL( mock, pthread_cond_signal(_) ).Times(3).WillOnce( Return( 1 ) ).WillRepeatedly( Return( 0 ) );
    EXPECT_CALL( mock, pthread_mutex_unlock(_) ).Times(2).WillOnce( Return( 1 ) ).WillRepeatedly( Return( 0 ) );

    // Check the agent is ready
    ac->ready = FALSE;
    ASSERT_EQ( agent_execute(NULL), AGENT_RET_EINIT );
    // Check the agent is busy
    ac->ready = TRUE;
    ASSERT_EQ( agent_execute(NULL), AGENT_RET_EBUSY );
    // Check set the right action
    ac->ready = TRUE;
    ASSERT_EQ( agent_execute(NULL), AGENT_RET_EBUSY );
    ac->ready = TRUE;
    ASSERT_EQ( agent_execute(NULL), AGENT_RET_EFAIL );
    // Check wake up agent main thread
    ac->ready = TRUE;
    ASSERT_EQ( agent_execute("Agent is Good"), AGENT_RET_EFAIL );
    // Check release lock 
    ac->ready = TRUE;
    ASSERT_EQ( agent_execute("Agent is GOOD"), AGENT_RET_EFAIL );
    // Complete
    ac->ready = TRUE;
    ASSERT_EQ( agent_execute("Agent is GOOD"), AGENT_RET_SUCCESS );
}

// Test agent_invoke
TEST ( AgentUnitTest, AGENT_INVOKE ) {
    GEN_MOCK_OBJECT( mock );

    EXPECT_CALL( mock, pthread_mutex_trylock(_) ).Times(6).WillOnce( Return( EBUSY ) ).WillRepeatedly( Return( 0 ) );
    EXPECT_CALL( mock, set_agent_action(AGENT_ACT_INVO) ).Times(5).WillOnce( Return( AGENT_RET_EBUSY ) ).WillOnce( Return( AGENT_RET_EFAIL ) ).WillRepeatedly( Return( AGENT_RET_SUCCESS ) );
    EXPECT_CALL( mock, pthread_cond_signal(_) ).Times(3).WillOnce( Return( 1 ) ).WillRepeatedly( Return( 0 ) );
    EXPECT_CALL( mock, pthread_mutex_unlock(_) ).Times(2).WillOnce( Return( 1 ) ).WillRepeatedly( Return( 0 ) );

    // Check the agent is ready
    ac->ready = FALSE;
    ASSERT_EQ( agent_invoke(NULL), AGENT_RET_EINIT );
    // Check the agent is busy
    ac->ready = TRUE;
    ASSERT_EQ( agent_invoke(NULL), AGENT_RET_EBUSY );
    // Check set the right action
    ac->ready = TRUE;
    ASSERT_EQ( agent_invoke(NULL), AGENT_RET_EBUSY );
    ac->ready = TRUE;
    ASSERT_EQ( agent_invoke(NULL), AGENT_RET_EFAIL );
    // Check wake up agent main thread
    ac->ready = TRUE;
    ASSERT_EQ( agent_invoke("Agent is Good"), AGENT_RET_EFAIL );
    // Check release lock 
    ac->ready = TRUE;
    ASSERT_EQ( agent_invoke("Agent is GOOD"), AGENT_RET_EFAIL );
    // Complete
    ac->ready = TRUE;
    ASSERT_EQ( agent_invoke("Agent is GOOD"), AGENT_RET_SUCCESS );

}

// Test agent_sync
TEST ( AgentUnitTest, AGENT_SYNC ) {
    GEN_MOCK_OBJECT( mock );

    EXPECT_CALL( mock, pthread_mutex_trylock(_) ).Times(6).WillOnce( Return( EBUSY ) ).WillRepeatedly( Return( 0 ) );
    EXPECT_CALL( mock, set_agent_action(AGENT_ACT_SYNC) ).Times(5).WillOnce( Return( AGENT_RET_EBUSY ) ).WillOnce( Return( AGENT_RET_EFAIL ) ).WillRepeatedly( Return( AGENT_RET_SUCCESS ) );
    EXPECT_CALL( mock, pthread_cond_signal(_) ).Times(3).WillOnce( Return( 1 ) ).WillRepeatedly( Return( 0 ) );
    EXPECT_CALL( mock, pthread_mutex_unlock(_) ).Times(2).WillOnce( Return( 1 ) ).WillRepeatedly( Return( 0 ) );

    // Check the agent is ready
    ac->ready = FALSE;
    ASSERT_EQ( agent_sync(), AGENT_RET_EINIT );
    // Check the agent is busy
    ac->ready = TRUE;
    ASSERT_EQ( agent_sync(), AGENT_RET_EBUSY );
    // Check set the right action
    ac->ready = TRUE;
    ASSERT_EQ( agent_sync(), AGENT_RET_EBUSY );
    ac->ready = TRUE;
    ASSERT_EQ( agent_sync(), AGENT_RET_EFAIL );
    // Check wake up agent main thread
    ac->ready = TRUE;
    ASSERT_EQ( agent_sync(), AGENT_RET_EFAIL );
    // Check release lock 
    ac->ready = TRUE;
    ASSERT_EQ( agent_sync(), AGENT_RET_EFAIL );
    // Complete
    ac->ready = TRUE;
    ASSERT_EQ( agent_sync(), AGENT_RET_SUCCESS );
}

// Test agent_logfile
TEST ( AgentUnitTest, AGENT_LOGFILE ) {
    GEN_MOCK_OBJECT( mock );

    EXPECT_CALL( mock, pthread_mutex_trylock(_) ).Times(6).WillOnce( Return( EBUSY ) ).WillRepeatedly( Return( 0 ) );
    EXPECT_CALL( mock, set_agent_action(AGENT_ACT_LOGF) ).Times(5).WillOnce( Return( AGENT_RET_EBUSY ) ).WillOnce( Return( AGENT_RET_EFAIL ) ).WillRepeatedly( Return( AGENT_RET_SUCCESS ) );
    EXPECT_CALL( mock, pthread_cond_signal(_) ).Times(3).WillOnce( Return( 1 ) ).WillRepeatedly( Return( 0 ) );
    EXPECT_CALL( mock, pthread_mutex_unlock(_) ).Times(2).WillOnce( Return( 1 ) ).WillRepeatedly( Return( 0 ) );

    // Check the agent is ready
    ac->ready = FALSE;
    ASSERT_EQ( agent_logfile(NULL), AGENT_RET_EINIT );
    // Check the agent is busy
    ac->ready = TRUE;
    ASSERT_EQ( agent_logfile(NULL), AGENT_RET_EBUSY );
    // Check set the right action
    ac->ready = TRUE;
    ASSERT_EQ( agent_logfile(NULL), AGENT_RET_EBUSY );
    ac->ready = TRUE;
    ASSERT_EQ( agent_logfile(NULL), AGENT_RET_EFAIL );
    // Check wake up agent main thread
    ac->ready = TRUE;
    ASSERT_EQ( agent_logfile("Agent is GOOD"), AGENT_RET_EFAIL );
    // Check release lock 
    ac->ready = TRUE;
    ASSERT_EQ( agent_logfile("Agent is GOOD"), AGENT_RET_EFAIL );
    // Complete
    ac->ready = TRUE;
    ASSERT_EQ( agent_logfile("Agent is GOOD"), AGENT_RET_SUCCESS );
}

int call_back_call_back_int ( const char* ) {
    return mock_ptr->call_back_int();
}

// Test agent_init
TEST ( AgentUnitTest, AGENT_INIT ) {
    GEN_MOCK_OBJECT( mock );
    ON_CALL( mock, set_agent_action(_)).WillByDefault(Return(AGENT_RET_SUCCESS));
    
    // Check the initialization is going
    ac->act.type = AGENT_ACT_INIT;
    ASSERT_EQ( _agent_init(NULL, 8888, NULL), AGENT_RET_EBUSY );
    // Check the initialization is completed
    ac->act.type = AGENT_ACT_IDLE;
    ac->ready = TRUE;
    ASSERT_EQ( _agent_init(NULL, 8888, NULL), AGENT_RET_SUCCESS );
    // Check the if( forward_port(fwd_config) == 0 ) works
    ac->act.type = AGENT_ACT_IDLE;
    ac->ready = FALSE;
    EXPECT_CALL( mock, call_back_int() ).WillRepeatedly( Return( 1 ) );
    EXPECT_CALL( mock, agent_printf(_) );
    EXPECT_CALL( mock, pthread_cond_destroy(_) );
    EXPECT_CALL( mock, pthread_mutex_destroy(_) ).Times(2);
    ASSERT_EQ( _agent_init(NULL, 0, &call_back_call_back_int), AGENT_RET_EFAIL );
    // Check initialize thread sync variable
    // pthread_mutex_init(&ac->act.mtx, NULL)
    EXPECT_CALL( mock, call_back_int() ).WillRepeatedly( Return(0) );
    EXPECT_CALL( mock, pthread_mutex_init(_,_) ).WillOnce( Return(1) );
    EXPECT_CALL( mock, pthread_cond_destroy(_) );
    EXPECT_CALL( mock, pthread_mutex_destroy(_) ).Times(2);
    ASSERT_EQ( _agent_init(NULL, 0, &call_back_call_back_int), AGENT_RET_EFAIL );
    // pthread_mutex_init(&ac->thread.mtx, NULL)
    EXPECT_CALL( mock, pthread_mutex_init(_,_) ).WillOnce( Return(0) ).WillOnce( Return(1) );
    EXPECT_CALL( mock, pthread_cond_destroy(_) );
    EXPECT_CALL( mock, pthread_mutex_destroy(_) ).Times(2);
    ASSERT_EQ( _agent_init(NULL, 0, &call_back_call_back_int), AGENT_RET_EFAIL );
    // pthread_cond_init(&ac->thread.cond, NULL)
    EXPECT_CALL( mock, pthread_mutex_init(_,_) ).WillRepeatedly( Return(0) );
    EXPECT_CALL( mock, pthread_cond_init(_,_) ).WillOnce( Return(1) );
    EXPECT_CALL( mock, pthread_cond_destroy(_) );
    EXPECT_CALL( mock, pthread_mutex_destroy(_) ).Times(2);
    ASSERT_EQ( _agent_init(NULL, 0, &call_back_call_back_int), AGENT_RET_EFAIL );
    // pthread_create( &ac->thread.tid, NULL, agent_client_mainloop, NULL )
    EXPECT_CALL( mock, pthread_cond_init(_,_) ).WillRepeatedly( Return(0) );
    EXPECT_CALL( mock, set_agent_action(AGENT_ACT_INIT) );
    EXPECT_CALL( mock, pthread_create(_,_,_,_) ).WillOnce( Return(1) );
    EXPECT_CALL( mock, pthread_cond_destroy(_) );
    EXPECT_CALL( mock, pthread_mutex_destroy(_) ).Times(2);
    ASSERT_EQ( _agent_init(NULL, 0, &call_back_call_back_int), AGENT_RET_EFAIL );
    // complete
    EXPECT_CALL( mock, set_agent_action(AGENT_ACT_INIT) );
    EXPECT_CALL( mock, pthread_create(_,_,_,_) ).WillOnce( Return(0) );
    ASSERT_EQ( _agent_init(NULL, 0, &call_back_call_back_int), AGENT_RET_SUCCESS );
}

// Test agent_reset
TEST ( AgentUnitTest, AGENT_RESET ) {
    GEN_MOCK_OBJECT( mock );
    // FAIL 
    ac->fwd_port = 5566;
    EXPECT_CALL( mock, agent_cleanup() );
    EXPECT_CALL( mock, agent_init(_,5566,_) ).WillOnce( Return(AGENT_RET_EFAIL) );
    ASSERT_EQ( agent_reset(NULL), AGENT_RET_EFAIL );
    // SUCCESS 
    ac->fwd_port = 5566;
    EXPECT_CALL( mock, agent_cleanup() );
    EXPECT_CALL( mock, agent_init(_,5566,_) ).WillOnce( Return(AGENT_RET_SUCCESS) );
    ASSERT_EQ( agent_reset(NULL), AGENT_RET_SUCCESS );
}


