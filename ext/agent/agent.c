/*
 *  Windows in-VM agent implementation
 *
 *  Copyright (c)   2016 Chiawei Wang
 *                  2016 Chuan-Hua, Cheng
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
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <pthread.h>
#include <monitor/monitor.h>

#include <sys/types.h>
#include <sys/socket.h>

#include <arpa/inet.h>
#include "agent.h"

struct agent_context {
    
    // member for QEMU - agent communication
    struct {
        pthread_t       tid;
        pthread_mutex_t mtx;
        pthread_cond_t  cond;
    } thread;

    // member for agent action
    struct {
        pthread_mutex_t mtx;
        MBA_AGENT_ACTION type;
        char cmdline[SZ_MAX_COMMAND];
        char dst_path[SZ_MAX_FILEPATH];
        char src_path[SZ_MAX_FILEPATH];
    } act;

    int      sock;
    uint16_t fwd_port;

    bool ready;

    Monitor* mon;
};
typedef struct agent_context agent_context;

agent_context ac[1];

/// Private function
/// These function should not be called directly, especially in the QEMU emulation thread.
/// Otherwise, the emulation thread blocks when performing network I/O with the guest.
/// But the guest has no chance to response the network I/O since the blocked emulation.

/// Cleanup the resource stored in the agent context
/// Return none
static void agent_cleanup( void ) {

    // thread cleanup
    pthread_cancel( ac->thread.tid );
    pthread_join( ac->thread.tid, NULL );

    // lock cleanup
    while( pthread_mutex_trylock(&ac->thread.mtx) == EBUSY )
        pthread_mutex_unlock( &ac->thread.mtx );
    pthread_mutex_unlock( &ac->thread.mtx );
    pthread_mutex_destroy( &ac->thread.mtx );

    while( pthread_mutex_trylock(&ac->act.mtx) == EBUSY )
        pthread_mutex_unlock( &ac->act.mtx );
    pthread_mutex_unlock( &ac->act.mtx );
    pthread_mutex_destroy( &ac->act.mtx );
    
    // context structure zero-out
    bzero( ac, sizeof(agent_context) );
}

/// (A)gent (S)erver write
/// A wrapper of write(), performing additional socket connection check
/// to ensure the communication between QEMU and the in-VM agent server
///
///     \param  sock_fd     The file descriptor of socket
///     \param  buf         Pointer to the buffer to be written
///     \param  count       Count in bytes of buf to be written
///
/// Return bytes written, <= 0 if any error occured
static ssize_t as_write( int sock_fd, void* buf, size_t count ) {

    ssize_t n_wbytes = write( sock_fd, buf, count );

    // 0: connection orderly closed, -1: error
    if( n_wbytes <= 0 ) {
        agent_printf( "The connection to the agent server is broken while writing\n" );
        agent_cleanup();
    }
    return n_wbytes;
}

/// (A)gent (S)erver read
/// A wrapper of read(), performing additional socket connection check
/// to ensure the communication between QEMU and the in-VM agent server
///
///     \param  sock_fd     The file descriptor of socket
///     \param  buf         Pointer to the buffer to be read
///     \param  count       Count in bytes of buf to be read
///
/// Return bytes read, <= 0 if any error occured
static ssize_t as_read( int sock_fd, void* buf, size_t count ) {

    ssize_t n_rbytes = read( sock_fd, buf, count );

    // 0: connection orderly closed, -1: error
    if( n_rbytes <= 0 ) {
        agent_printf( "The connection to the agent server is broken while reading\n" );
        agent_cleanup();
    }
    return n_rbytes;
}

/// Synchronously set the current agent action.
/// The agent action type is shared by the QEMU emulation & Agent threads.
/// Thereby, a mutex(lock) is required to perform mutually exclusive access.
/// 
///     \param act_type     a enum value indicating the action type
/// 
/// Return none
static void set_agent_action( MBA_AGENT_ACTION act_type ) {
    pthread_mutex_lock( &ac->act.mtx );
    ac->act.type = act_type;
    pthread_mutex_unlock( &ac->act.mtx );
}

/// Import a host file into guest
/// Return none
static MBA_AGENT_RETURN import_host_file( void ) {

    int fd = -1;

    struct stat fstat;
    uint64_t    fsize;
    char        fbuf[SZ_MAX_FILECHUNK];
    char*       fptr;

    char cmd_emit[SZ_MAX_COMMAND];

    int n_rbytes;
    int n_wbytes;

    const char* dst_path = ac->act.dst_path;
    const char* src_path = ac->act.src_path;

    fd = open( src_path, O_RDONLY );
    if( fd == -1 ) {
        agent_printf( "Failed to open '%s' for file import\n", src_path );
        goto impo_fail;
    }

    // get file size
    if( stat(src_path, &fstat) == -1 ) {
        agent_printf( "Failed to get the file status of '%s'\n", src_path );
        goto impo_fail;
    }
    fsize = fstat.st_size;

    // construct the final command for agent server
    bzero( cmd_emit, SZ_MAX_COMMAND );
    snprintf( cmd_emit, SZ_MAX_COMMAND, "impo %s", dst_path );
    
    // emit command
    n_wbytes = as_write( ac->sock, cmd_emit, SZ_MAX_COMMAND );
    if( n_wbytes != SZ_MAX_COMMAND ) {
        agent_printf( "Failed to emit command '%s'\n", cmd_emit );
        goto impo_fail;
    }

    // send file size
    n_wbytes = as_write( ac->sock, &fsize, sizeof(uint64_t) );
    if( n_wbytes != sizeof(uint64_t) ) {
        agent_printf( "Failed to send file size: %u\n", fsize );
        goto impo_fail;
    }

    // read & send file content to server
    while( fsize ) {

        // read local file
        n_rbytes = read( fd, fbuf, SZ_MAX_FILECHUNK );
        if( n_rbytes == -1 ) {
            agent_printf( "Failed to read content of '%s'\n", src_path );
            goto impo_fail;
        }

        fsize -= n_rbytes;

        // send to agent server
        fptr = fbuf;
        while( n_rbytes ) {
            n_wbytes = as_write( ac->sock, fptr, n_rbytes );
            if( n_wbytes <= 0 ) {
                agent_printf( "Failed to send file content\n" );
                goto impo_fail;
            }

            n_rbytes -= n_wbytes;
            fptr     += n_wbytes;
        }
    
    }

    close( fd );
    return AGENT_RET_SUCCESS;

impo_fail:
    if( fd != -1 )
        close( fd );
    return AGENT_RET_EFAIL;
}

/// Export a guest file to host
/// Return none
static MBA_AGENT_RETURN export_guest_file( void ) {
    
    char cmd_emit[SZ_MAX_COMMAND];

    uint64_t fsize;
    
    char  fbuf[SZ_MAX_FILECHUNK];
    char* fptr;

    int fd = -1;

    int n_rbytes;
    int n_wbytes;

    const char* dst_path = ac->act.dst_path;
    const char* src_path = ac->act.src_path;

    // construct the final command for agent server
    bzero( cmd_emit, SZ_MAX_COMMAND );
    snprintf( cmd_emit, SZ_MAX_COMMAND, "expo %s", src_path );

    // emit command
    n_wbytes = as_write( ac->sock, cmd_emit, SZ_MAX_COMMAND );
    if( n_wbytes != SZ_MAX_COMMAND ) {
        agent_printf( "Failed to emit command '%s'\n", cmd_emit );
        goto expo_fail;
    }

    // receive file size
    n_rbytes = as_read( ac->sock, &fsize, sizeof(uint64_t) );
    if( n_rbytes != sizeof(uint64_t) ) {
        agent_printf( "Failed to receive file size\n" );
        goto expo_fail;
    }

    // open destination file
    fd = open( dst_path, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH );
    if( fd == -1 ) {
        agent_printf( "Failed to open '%s' for file export\n", dst_path );
        goto expo_fail;
    }

    // receive & store file content
    while( fsize ) {

        // measure the maximum bytes should be read
        n_rbytes = (fsize < SZ_MAX_FILECHUNK) ? fsize : SZ_MAX_FILECHUNK;

        // receive file content from agent server
        n_rbytes = as_read( ac->sock, fbuf, n_rbytes );
        if( n_rbytes <= 0 ) {
            agent_printf( "Failed to receive file content\n" );
            goto expo_fail;
        }

        fsize -= n_rbytes;

        // store to local file
        fptr = fbuf;
        while( n_rbytes ) {
            n_wbytes = write( fd, fptr, n_rbytes );
            if( n_wbytes == -1 ) {
                agent_printf( "Failed to store file content\n" );
                goto expo_fail;
            }

            n_rbytes -= n_wbytes;
            fptr     += n_wbytes;
        }
    }

    close( fd );
    return AGENT_RET_SUCCESS;
 
expo_fail:
    if( fd != -1 )
        close( fd );
    return AGENT_RET_EFAIL;
}

/// Execute a guest command and perform interactive stdin/stdout
/// Return none
static MBA_AGENT_RETURN execute_guest_cmd_return( void ) {
    
    char exec_rdy[sizeof(MSG_EXEC_READY)];
    char cmd_emit[SZ_MAX_COMMAND];

    uint32_t msize;
    char     msg_chunk[4096];

    int n_rbytes;
    int n_wbytes;

    const char* cmdline = ac->act.cmdline;

    int i;

    // construct the final command for agent server
    bzero( cmd_emit, SZ_MAX_COMMAND );
    snprintf( cmd_emit, SZ_MAX_COMMAND, "exec %s", cmdline );

    // emit command
    n_wbytes = as_write( ac->sock, cmd_emit, SZ_MAX_COMMAND );
    if( n_wbytes != SZ_MAX_COMMAND ) {
        agent_printf( "Failed to emit command '%s'\n", cmd_emit );
        goto exec_fail;
    }
    
    // read the constant string "EXEC_READY"
    bzero( exec_rdy, sizeof(MSG_EXEC_READY) );
    n_rbytes = as_read( ac->sock, exec_rdy, sizeof(MSG_EXEC_READY) );

    if( n_rbytes != sizeof(MSG_EXEC_READY) || strcmp(exec_rdy, MSG_EXEC_READY) != 0 ) {
        agent_printf( "Failed to receive the ready signal for agent 'execute' action\n" );
        goto exec_fail;
    }

    // infinite loop to receive guest output
    msize = 0;
    while( true ) {

        // get output message size
        n_rbytes = as_read( ac->sock, &msize, sizeof(uint32_t) );
        if( n_rbytes != sizeof(uint32_t) ) 
            goto exec_fail;

        if( msize == 0 )
            break;

        // get & print message(may not be null-terminated) content
        while( msize ) {

            // measure the maximum bytes should be read
            n_rbytes = (msize < sizeof(msg_chunk)) ? msize : sizeof(msg_chunk); 

            // read output message
            n_rbytes = as_read( ac->sock, msg_chunk, n_rbytes );

            if( n_rbytes <= 0 ) {
                agent_printf( "Failed to revceive agent exec output\n" );
                goto exec_fail;
            }

            for( i = 0; i < n_rbytes; ++i )
                agent_printf( "%c", msg_chunk[i] );
            msize -= n_rbytes;
        }
    }   

    return AGENT_RET_SUCCESS;

exec_fail:
    return AGENT_RET_EFAIL;
}

/// Execute a guest command without expecting the output
/// Return none
static MBA_AGENT_RETURN execute_guest_cmd_noreturn( void ) {

    char cmd_emit[SZ_MAX_COMMAND];

    int n_wbytes;
    
    const char* cmdline = ac->act.cmdline;

    // construct the final command for agent server
    bzero( cmd_emit, SZ_MAX_COMMAND );
    snprintf( cmd_emit, SZ_MAX_COMMAND, "invo %s", cmdline );

    // emit command
    n_wbytes = as_write( ac->sock, cmd_emit, SZ_MAX_COMMAND );
    if( n_wbytes != SZ_MAX_COMMAND ) {
        agent_printf( "Failed to emit command '%s'\n", cmd_emit );
        goto invo_fail;
    }

    return AGENT_RET_SUCCESS;

invo_fail:
    return AGENT_RET_EFAIL;
}

/// Export the agent server log to host
/// Return none
static MBA_AGENT_RETURN export_agent_log( void ) {

    char cmd_emit[SZ_MAX_COMMAND];

    uint64_t fsize;

    char  fbuf[SZ_MAX_FILECHUNK];
    char* fptr;

    int fd = -1;

    int n_rbytes;
    int n_wbytes;

    const char* dst_path = ac->act.dst_path;

    // construct the final command for agent server
    bzero( cmd_emit, SZ_MAX_COMMAND );
    snprintf( cmd_emit, SZ_MAX_COMMAND, "logf" );

    // emit command
    n_wbytes = as_write( ac->sock, cmd_emit, SZ_MAX_COMMAND );
    if( n_wbytes != SZ_MAX_COMMAND ) {
        agent_printf( "Failed to emit command '%s'\n", cmd_emit );
        goto logf_fail;
    }

    // receive log file size
    n_rbytes = as_read( ac->sock, &fsize, sizeof(uint64_t) );
    if( n_rbytes != sizeof(uint64_t) ) {
        agent_printf( "Failed to receive log file size\n" );
        goto logf_fail;
    }

    // open destination file to store the log
    fd = open( dst_path, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH );
    if( fd == -1 ) {
        agent_printf( "Failed to open '%s' for agent log export\n", dst_path );
        goto logf_fail;
    }

    // receive & store file content
    while( fsize ) {

        // measure the maximum bytes should be read
        n_rbytes = (fsize < SZ_MAX_FILECHUNK) ? fsize : SZ_MAX_FILECHUNK;

        // receive file content from agent server
        n_rbytes = as_read( ac->sock, fbuf, n_rbytes );
        if( n_rbytes <= 0 ) {
            agent_printf( "Failed to receive file content\n" );
            goto logf_fail;
        }

        fsize -= n_rbytes;

        // store to local file
        fptr = fbuf;
        while( n_rbytes ) {
            n_wbytes = write( fd, fptr, n_rbytes );
            if( n_wbytes == -1 ) {
                agent_printf( "Failed to store file content\n" );
                goto logf_fail;
            }

            n_rbytes -= n_wbytes;
            fptr     += n_wbytes;
        }
    }

    close( fd );
    return AGENT_RET_SUCCESS;

logf_fail:
    if( fd != -1 )
        close( fd );
    return AGENT_RET_EFAIL;
}

/// Receive agent server ack message
/// Return none
static void show_server_ack( void ) {

    int n_rbytes;

    char ack_msg[SZ_MAX_COMMAND];

    bzero( ack_msg, sizeof(ack_msg) );
    n_rbytes = as_read( ac->sock, ack_msg, sizeof(MSG_ACK_PREFIX) );
    if( n_rbytes == sizeof(MSG_ACK_PREFIX) )
       agent_printf( "%s", ack_msg );

    bzero( ack_msg, sizeof(ack_msg) );
    n_rbytes = as_read( ac->sock, ack_msg, SZ_MAX_COMMAND );
    if( n_rbytes == SZ_MAX_COMMAND )
        agent_printf( "%s\n", ack_msg );
} 

/// Connect agent server via localhost redirected port
/// Return socket descriptor on success, -1 otherwise
static int connect_agent_server( void ) {

    int    sock;
    struct sockaddr_in server_addr;

    bzero( &server_addr, sizeof(server_addr) );

    server_addr.sin_family = AF_INET;
    server_addr.sin_port   = htons(ac->fwd_port);

    sock = socket( AF_INET,SOCK_STREAM, 0 );
    if( sock == -1 )
        return -1;

    inet_pton( AF_INET, "127.0.0.1", &server_addr.sin_addr );
    if( connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1 )
        return -1;

    return sock;
}

/// The main loop of the agent thread.
/// Serving as the dispatcher to the action handler
/// Return none
static void* agent_client_mainloop( void* null_arg ) {
    
    MBA_AGENT_RETURN ret;

    // connect to agent server
    ac->sock = connect_agent_server();
    if( ac->sock == -1 ) {
        agent_printf( "Failed to connect to agent server\n" );
        return NULL;
    }

    // agent is ready to go 
    ac->ready = true;
    set_agent_action( AGENT_ACT_IDLE );

    while( true ) {

        pthread_mutex_lock( &ac->thread.mtx );

        // hold until an agent command is issued
        while( ac->act.type == AGENT_ACT_IDLE )
            pthread_cond_wait( &ac->thread.cond, &ac->thread.mtx );

        // dispatcher
        ret = AGENT_RET_EFAIL;
        switch( ac->act.type ) {

            case AGENT_ACT_IMPO:
                ret = import_host_file();
                break;

            case AGENT_ACT_EXPO:
                ret = export_guest_file();
                break;

            case AGENT_ACT_EXEC:
                ret = execute_guest_cmd_return();
                break;

            case AGENT_ACT_INVO:
                ret = execute_guest_cmd_noreturn();
                break;

            case AGENT_ACT_LOGF:
                ret = export_agent_log();
                break;

            default:
                agent_printf( "Unkown agent action type: %d\n", ac->act.type );
                break;
        }

        if( ret == AGENT_RET_SUCCESS ) {
            show_server_ack();
        }
        else
            agent_printf( "Previous command failed. Error code: %d\n", ret );
    
        // reset action
        set_agent_action( AGENT_ACT_IDLE );
    
        pthread_mutex_unlock( &ac->thread.mtx );
    } 
}


/// Public API
/// Each API should be named with the 'agent_' prefix.
/// Note that an agent thread (via agent_init()) should exists to co-work with
inline bool agent_is_ready( void ) {
    return ac->ready;
}

bool agent_is_exec( void ) {

    bool ret = false;

    pthread_mutex_lock( &ac->act.mtx );
    ret = (ac->act.type == AGENT_ACT_EXEC);
    pthread_mutex_unlock( &ac->act.mtx );

    return ret;
}

void agent_printf( const char* fmt, ... ) {

    va_list args;

    if( ac->mon == NULL )
        return;
    
    va_start( args, fmt );
    monitor_vprintf( ac->mon, fmt, args );
    va_end( args );
}

void agent_handle_exec_command( const char* cmdline ) {

    char cmd_emit[SZ_MAX_COMMAND];

    if( !agent_is_ready() || !agent_is_exec() )
        return;

    bzero( cmd_emit, SZ_MAX_COMMAND );
    strncpy( cmd_emit, cmdline, SZ_MAX_COMMAND - 1 );

    // append the newline character
    strcat( cmd_emit, "\n" );

    // forward execute command to agent server
    as_write( ac->sock, cmd_emit, SZ_MAX_COMMAND );
}       

MBA_AGENT_RETURN agent_import( const char* dst_path, const char* src_path ) {

    if( !agent_is_ready() )
        return AGENT_RET_EINIT;

    // get thread lock to setup IMPOrt action parameters
    if( pthread_mutex_lock( &ac->thread.mtx ) == EBUSY )
        return AGENT_RET_EBUSY;

    // setup 'import' action
    set_agent_action( AGENT_ACT_IMPO );

    bzero( ac->act.dst_path, SZ_MAX_FILEPATH );
    bzero( ac->act.src_path, SZ_MAX_FILEPATH );

    strncpy( ac->act.dst_path, dst_path, SZ_MAX_FILEPATH );
    strncpy( ac->act.src_path, src_path, SZ_MAX_FILEPATH );

    // wake up agent main thread
    if( pthread_cond_signal(&ac->thread.cond) != 0 )
        return AGENT_RET_EFAIL;

    // release lock
    if( pthread_mutex_unlock(&ac->thread.mtx) != 0 )
        return AGENT_RET_EFAIL;

    return AGENT_RET_SUCCESS;
}

MBA_AGENT_RETURN agent_export( const char* dst_path, const char* src_path ) {
 
    if( !agent_is_ready() )
        return AGENT_RET_EINIT;

    // get thread lock to setup EXPOrt action parameters
    if( pthread_mutex_trylock( &ac->thread.mtx) == EBUSY )
        return AGENT_RET_EBUSY;

    // setup export action
    set_agent_action( AGENT_ACT_EXPO );

    bzero( ac->act.dst_path, SZ_MAX_FILEPATH );
    bzero( ac->act.src_path, SZ_MAX_FILEPATH );

    strncpy( ac->act.dst_path, dst_path, SZ_MAX_FILEPATH );
    strncpy( ac->act.src_path, src_path, SZ_MAX_FILEPATH );

    // wake up agent thread
    if( pthread_cond_signal(&ac->thread.cond) != 0 )
        return AGENT_RET_EFAIL;

    // release lock
    if( pthread_mutex_unlock(&ac->thread.mtx) != 0 )
        return AGENT_RET_EFAIL;

    return AGENT_RET_SUCCESS;
} 

MBA_AGENT_RETURN agent_execute( const char* cmdline ) {

    if( !agent_is_ready() )
        return AGENT_RET_EINIT;

    // get thread lock to setup EXECute action parameters
    if( pthread_mutex_trylock( &ac->thread.mtx ) == EBUSY )
        return AGENT_RET_EBUSY;

    // setup execute action
    set_agent_action( AGENT_ACT_EXEC );

    bzero( ac->act.cmdline, SZ_MAX_COMMAND );
    strncpy( ac->act.cmdline, cmdline, SZ_MAX_COMMAND );

    // wake up agent thread
    if( pthread_cond_signal(&ac->thread.cond) != 0 )
        return AGENT_RET_EFAIL;

    // release lock
    if( pthread_mutex_unlock(&ac->thread.mtx) != 0 )
        return AGENT_RET_EFAIL;

    return AGENT_RET_SUCCESS;   
} 

MBA_AGENT_RETURN agent_invoke( const char* cmdline ) {
    
    if( !agent_is_ready() )
        return AGENT_RET_EINIT;

    // get thread lock to setup INVOke action parameters
    if( pthread_mutex_trylock( &ac->thread.mtx ) == EBUSY )
        return AGENT_RET_EBUSY;

    // setup invoke action
    set_agent_action( AGENT_ACT_INVO );

    bzero( ac->act.cmdline, SZ_MAX_COMMAND );
    strncpy( ac->act.cmdline, cmdline, SZ_MAX_COMMAND );

    // wake up agent thread
    if( pthread_cond_signal(&ac->thread.cond) != 0 )
        return AGENT_RET_EFAIL;

    // release lock
    if( pthread_mutex_unlock(&ac->thread.mtx) != 0 )
        return AGENT_RET_EFAIL;

    return AGENT_RET_SUCCESS;   
}

MBA_AGENT_RETURN agent_logfile( const char* dst_path ) {

    if( !agent_is_ready() )
        return AGENT_RET_EINIT;

    // get thread lock to setup LOG File cat action
    if( pthread_mutex_trylock( &ac->thread.mtx ) == EBUSY )
        return AGENT_RET_EBUSY;

    // setup log file action
    set_agent_action( AGENT_ACT_LOGF );

    bzero( ac->act.dst_path, SZ_MAX_FILEPATH );
    strncpy( ac->act.dst_path, dst_path, SZ_MAX_FILEPATH );

    // wake up agent thread
    if( pthread_cond_signal(&ac->thread.cond) != 0 )
        return AGENT_RET_EFAIL;

    // release lock
    if( pthread_mutex_unlock(&ac->thread.mtx) != 0 )
        return AGENT_RET_EFAIL;

    return AGENT_RET_SUCCESS;
}

MBA_AGENT_RETURN agent_init( Monitor *mon, uint16_t server_fwd_port ) {

    if( ac->act.type == AGENT_ACT_INIT )
        return AGENT_RET_EBUSY;

    if( agent_is_ready() )
        return AGENT_RET_SUCCESS;

    // reset agent context
    bzero( ac, sizeof(agent_context) );

    // setup monitor for agent aessage output to QEMU monitor
    ac->mon = mon;

    // connect to in-VM agent server via the forwarding port
    ac->fwd_port = server_fwd_port;

    // initialize thread sync var
    if( pthread_mutex_init(&ac->act.mtx, NULL) != 0 )
        goto init_fail;

    if( pthread_mutex_init(&ac->thread.mtx, NULL) != 0 )
        goto init_fail;

    if( pthread_cond_init(&ac->thread.cond, NULL) != 0 )
        goto init_fail;

    // now we are in initializing action
    set_agent_action( AGENT_ACT_INIT );

    // spawn agent thread, the agent is ready until the thread connects the in-VM agent server
    if( pthread_create( &ac->thread.tid, NULL, agent_client_mainloop, NULL ) != 0 ) 
        goto init_fail;

    return AGENT_RET_SUCCESS;

init_fail:
    pthread_mutex_destroy( &ac->act.mtx );
    pthread_mutex_destroy( &ac->thread.mtx );
    pthread_cond_destroy( &ac->thread.cond );

    return AGENT_RET_EFAIL;
}
