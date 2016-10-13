/*
 *  De-coupled Information Flow Tracking (DIFT) test cases
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

extern "C" {
#include "../agent.c"

}

struct hook_functions {
    virtual ~hook_functions() {};

    virtual void exit( int ) = 0;
    virtual int pthread_create(pthread_t*, const pthread_attr_t*, void*(*)(void*), void* ) = 0;
    virtual int pthread_mutex_init(pthread_mutex_t*, const pthread_mutexattr_t * ) = 0;
    virtual int pthread_cond_init(pthread_cond_t*, const pthread_condattr_t*) = 0;
    virtual FILE* fopen(const char *, const char *) = 0;
    virtual int write( int, void*, int ) = 0;
    virtual int read( int, void*, int ) = 0;

    virtual void agent_cleanup( void ) = 0;
    virtual ssize_t as_write( int, void*, size_t ) = 0;
    virtual ssize_t as_read( int, void*, size_t ) = 0;
    virtual MBA_AGENT_RETURN import_host_file( void ) = 0;
    virtual MBA_AGENT_RETURN export_guest_file( void ) = 0;
    virtual MBA_AGENT_RETURN execute_guest_cmd_return( void ) = 0;
    virtual MBA_AGENT_RETURN execute_guest_cmd_noreturn( void ) = 0;
    virtual MBA_AGENT_RETURN export_agent_log( void ) = 0;
    virtual int connect_agent_server( void ) = 0;
    virtual bool agent_is_ready( void ) = 0;
    virtual bool agent_is_exec( void ) = 0;
    virtual void agent_printf( const char*, ... ) = 0;
    virtual MBA_AGENT_RETURN agent_import( const char*, const char* ) = 0;
    virtual MBA_AGENT_RETURN agent_export( const char*, const char* ) = 0;
    virtual MBA_AGENT_RETURN agent_execute( const char* ) = 0;
    virtual MBA_AGENT_RETURN agent_invoke( const char* ) = 0;
    virtual MBA_AGENT_RETURN agent_logfile( const char* ) = 0;
    virtual MBA_AGENT_RETURN agent_init( Monitor*, uint16_t ) = 0;
};

struct mock_functions : hook_functions {
    MOCK_METHOD1( exit, void( int ) )
    MOCK_METHOD4( pthread_create, int(pthread_t*, const pthread_attr_t*, void*(*)(void*), void* ) )
    MOCK_METHOD2( fopen, FILE*(const char *, const char *) )
    MOCK_METHOD3( write, int(int, void*, int) )
    MOCK_METHOD3( read, int(int, void*, int) )

    MOCK_METHOD0( agent_cleanup, void( void ) )
    MOCK_METHOD3( as_write, ssize_t( int, void*, size_t ) )
    MOCK_METHOD3( as_read, ssize_t( int, void*, size_t ) )
    MOCK_METHOD0( import_host_file, MBA_AGENT_RETURN( void ) )
    MOCK_METHOD0( export_guest_fil, MBA_AGENT_RETURN( void ) )
    MOCK_METHOD0( execute_guest_cmd_return, MBA_AGENT_RETURN( void ) )
    MOCK_METHOD0( execute_guest_cmd_noreturn, MBA_AGENT_RETURN( void ) )
    MOCK_METHOD0( export_agent_log, MBA_AGENT_RETURN( void ) )
    MOCK_METHOD0( connect_agent_server, int( void ) )
    MOCK_METHOD0( agent_is_ready, bool( void ) )
    MOCK_METHOD0( agent_is_exec, bool( void ) )
    MOCK_METHOD2( agent_printf, void(const char*, ...) )
    MOCK_METHOD2( agent_import, MBA_AGENT_RETURN( const char*, const char* ) )
    MOCK_METHOD2( agent_export, MBA_AGENT_RETURN( const char*, const char* ) )
    MOCK_METHOD1( agent_execute, MBA_AGENT_RETURN( const char* ) )
    MOCK_METHOD1( agent_invoke, MBA_AGENT_RETURN( const char* ) )
    MOCK_METHOD1( agent_logfile, MBA_AGENT_RETURN( const char* ) )
    MOCK_METHOD2( agent_init, MBA_AGENT_RETURN( Monitor*, uint16_t ) )
} *mock_ptr;

#undef exit
#undef pthread_create
#undef fopen
#undef agent_clenup
#undef as_write
#undef as_read
#undef import_host_file
#undef export_guest_file
#undef execute_guest_cmd_return
#undef execute_guest_cmd_noreturn
#undef export_agent_log
#undef connect_agent_server
#undef agent_is_ready
#undef agent_is_exec
#undef agent_printf
#undef agent_import
#undef agent_export
#undef agent_execute
#undef agent_invoke
#undef agent_logfile
#undef agent_init

#define GEN_MOCK_OBJECT( x )  mock_functions x; mock_ptr = &x;
#define GEN_NICEMOCK_OBJECT( x )  NiceMock<mock_functions> x; mock_ptr = &x;

using namespace ::testing;
class FixtureAgentContextInitilizes : public Test {
protected:
    virtual void SetUp() {
        GEN_MOCK_OBJECT( mock );

        EXPECT_CALLL( mock, agent_is_ready() ).WillOnce( Return( true ) );
        EXPECT_CALLL( mock, pthread_mutex_init(_,_) ).WillOnce( Return( 0 ) );
        EXPECT_CALLL( mock, pthread_mutex_init(_,_) ).WillOnce( Return( 0 ) );
        EXPECT_CALLL( mock, pthread_cond_init(_,_) ).WillOnce( Return( 0 ) );
        EXPECT_CALLL( mock, pthread_create(_,_,_,_) ).WillOnce( Return( 0 ) );

        _agent_init(NULL, 8888);

    }
    virtual void TearDown() {
        // No recycable due to all initializations mocked
    }
}

// Static Functions Testing
TEST ( Agent_Init, WRITE_FAIL ) {
    GEN_MOCK_OBJECT( mock );
    
    EXPECT_CALL( mock, write(_,_,_) ).WillOnce( Return(-1) );
    EXPECT_CALL( mock, agent_printf("The connection to the agent server is broken while reading\n") ).Times( 1 )
    EXPECT_CALL( mock, agent_clenup() ).Times( 1 );

    _agent_init();
}





