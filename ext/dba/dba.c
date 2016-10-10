/*
 *  MBA Dynamic Behavior Analyzer, DBA implementation
 *
 *  Copyright (c)   2016 Chiawei Wang
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

#include "dba.h"
#include "dba_taint.h"
#include "dba_syscall.h"

#include "net/slirp.h"
#include "qmp-commands.h"
#include "include/utarray.h"

DBA_ERRNO dba_errno;

// next Task ID could be used, round by DBA_MAX_TASKS
static DBA_TID next_tid = 0;

// locally global array to store all DBA tasks
static dba_context* dba_tasks[DBA_MAX_TASKS] = { 0 };

// Private functions
static bool is_valid_tid( DBA_TID tid ) {
    return (tid >= 0 && tid < DBA_MAX_TASKS)? true : false;
}

static bool is_valid_sample( DBA_TID tid ) {

    dba_context* ctx;

    if( !is_valid_tid(tid) )
        return false;

    ctx = dba_tasks[tid];
    if( ctx == NULL )
        return false;

    if( 
        ctx->sample_hpath         == NULL 
     || strlen(ctx->sample_hpath) == 0
     || ctx->sample_gpath         == NULL
     || strlen(ctx->sample_gpath) == 0
     ) 
        return false;
    return true;
}

static bool is_task_configurable( DBA_TID tid ) {

    if( !is_valid_tid(tid) ) {
        dba_errno = DBA_ERR_INVALID_TID;
        return false;
    }

    // the config of a task is mutable only if the task is IDLE
    if( dba_tasks[tid]->state != DBA_TASK_IDLE ) {
        dba_errno = DBA_ERR_INVALID_TSTATE;
        return false;
    }

    return true;
}

static int toggle_taint_analysis( DBA_TID tid, CONTAMINATION_RECORD tag, bool is_enable ) {
    
    if( !is_task_configurable(tid) )
        return -1;

    if( is_enable && tag == 0 ) {
        dba_errno = DBA_ERR_DIFT_TAG;
        return -1;
    }

    dba_tasks[tid]->taint.is_enabled = is_enable;
    dba_tasks[tid]->taint.tag        = tag;
    return 0;
}

static int toggle_syscall_tracer( DBA_TID tid, bool is_enable ) {
    
    if( !is_task_configurable(tid) )
        return -1;

    dba_tasks[tid]->syscall.is_enabled = is_enable;
    return 0;
}

static DBA_TID get_available_tid( void ) {
 
    DBA_TID tid,
            null_tid = -1, // TID of the nearest unused task slot
            done_tid = -1; // TID of the nearest DONE task slot

    /// scan the whole tasks for usable task slot
    /// the slot with NULL DBA task object is the 
    /// priori choice and then the DONE DBA tasks
    tid = next_tid;
    do {
        if( dba_tasks[tid] == NULL ) {
            null_tid = tid;
            break;
        }

        /// we do not break in this case to 
        /// continue the search of NULL object
        if( dba_tasks[tid]->state == DBA_TASK_DONE && done_tid == -1 ) 
            done_tid = tid;

        tid = (tid + 1) % DBA_MAX_TASKS;
    } while( tid != next_tid );

    if( null_tid != -1 ) {
        next_tid = null_tid;
    }
    else if( done_tid != -1 ) {
    
        // reuse the DONE task slot
        json_object_put( dba_tasks[done_tid]->result );
        free( dba_tasks[done_tid] );
        dba_tasks[done_tid] = NULL;
        next_tid = done_tid;
    }
    else
        return -1;

    return next_tid;
}

// macro for agent action polling
#define AGENT_ACT( x ) \
    while( (aret = x) == AGENT_RET_EBUSY ) { asm volatile("pause"); } \
    while( !agent_is_idle() ) { asm volatile( "pause" ); }

// Wrpper of invoking sample and doing sync
// Return none
static void* invoke_sample( dba_context* ctx ) {

    MBA_AGENT_RETURN aret;

    // invoke sample
    AGENT_ACT( agent_invoke(ctx->sample_gpath) );

    // wait for the timer for sample execution
    sleep( ctx->sample_timer );

    // flush buffered read/write of the guest OS
    AGENT_ACT( agent_sync() );

    return NULL;
}

// dba task thread main
// retirn none
static void* dba_main_internal( void* ctx_arg ) {

    MBA_AGENT_RETURN aret;
    dba_context* ctx = ctx_arg;
    int ntm_cb_id = 0;

    ctx->state = DBA_TASK_BUSY;

    // import sample from host into guest & flush disk cache
    AGENT_ACT( agent_import(ctx->sample_gpath, ctx->sample_hpath) );
    AGENT_ACT( agent_sync() );

    if( ctx->taint.is_enabled ) {

        json_object_object_add( ctx->result, DBA_JSON_KEY_TAINT, json_object_new_object() );
        set_sample_tainted( ctx );

        // ---------- Add cb into nettramon ---------- //
        ntm_cb_id = nettramon_set_cb( &tainted_packet_cb, ctx_arg );
        // ---------- Start to capture packets ---------- //
        nettramon_start( NULL );
    }
    if( ctx->syscall.is_enabled ) {
        json_object_object_add( ctx->result, DBA_JSON_KEY_SYSCALL, json_object_new_object() );
        init_syscall_analysis( ctx );
    }

    // Start to execute sample
    invoke_sample( ctx );

    if( ctx->taint.is_enabled ) {
        // ---------- Delete the ntm call back funciton ---------- //
        nettramon_stop();
        nettramon_delete_cb( ntm_cb_id );

        enum_tainted_file( ctx );
    }
    if( ctx->syscall.is_enabled ) {
        clean_syscall_analysis( ctx );
    }

    ctx->state = DBA_TASK_DONE;

    monitor_printf( ctx->mon, "Task %d has finished\n", ctx->task_id );

    return NULL;
}
#undef AGENT_ACT

// Public APIs
DBA_TID dba_new_task( void ) {
    
    DBA_TID new_tid; 
    dba_context* ctx = NULL;

    new_tid = get_available_tid();
    if( new_tid == -1 ) {
        dba_errno = DBA_ERR_TASK_FULL;
        return -1; 
    }

    ctx = (dba_context*)malloc( sizeof(dba_context) );
    if( ctx == NULL ) {
        dba_errno = DBA_ERR_FAIL;
        return -1;
    }

    ctx->task_id = new_tid;
    ctx->result  = json_object_new_object();
    ctx->state   = DBA_TASK_IDLE;

    dba_tasks[new_tid] = ctx;

    return new_tid;
}

int dba_delete_task( DBA_TID tid ) {

    if( !is_valid_tid(tid) ) {
        dba_errno = DBA_ERR_INVALID_TID;
        return -1;
    }
    
    // nothing to do
    if( dba_tasks[tid] == NULL )
        return 0;

    // abort deletion if the task is BUSY
    if( dba_tasks[tid]->state == DBA_TASK_BUSY ) {
        dba_errno = DBA_ERR_INVALID_TSTATE;
        return -1;
    }

    json_object_put( dba_tasks[tid]->result );
    free( dba_tasks[tid] );

    dba_tasks[ tid ] = NULL;

    return 0;
}

const dba_context* dba_get_task_context( DBA_TID tid ) {

    if( !is_valid_tid(tid) ) {
        dba_errno = DBA_ERR_INVALID_TID;
        return NULL;
    }

    return dba_tasks[tid];
}

int dba_set_monitor( DBA_TID tid, Monitor* mon ) {

    if( !is_task_configurable(tid) ) 
        return -1;

    dba_tasks[tid]->mon = mon;
    return 0;
}

int dba_set_timer( DBA_TID tid, size_t seconds ) {

    if( !is_task_configurable(tid) ) 
        return -1;

    dba_tasks[tid]->sample_timer = seconds;
    return 0;
}

int dba_set_sample( DBA_TID tid, const char* path ) {
    
    dba_context* ctx;

    if( !is_task_configurable(tid) ) 
        return -1;

    if( strlen(path) > DBA_MAX_FILENAME ) {
        dba_errno = DBA_ERR_INVALID_SAMPLE;
        return -1;
    }

    ctx = dba_tasks[tid];

    // copy sample host path
    bzero( ctx->sample_hpath, DBA_MAX_FILENAME + 1 );
    strncpy( ctx->sample_hpath, path, DBA_MAX_FILENAME );

    // setup sample guest path
    bzero( ctx->sample_gpath, DBA_MAX_FILENAME + sizeof(DBA_GUEST_SAMPLE_DIR) );
    sprintf( ctx->sample_gpath, "%s%s", DBA_GUEST_SAMPLE_DIR, basename(ctx->sample_hpath) );

    return 0;
}

int dba_enable_syscall_trace( DBA_TID tid ) {
    return toggle_syscall_tracer( tid, true );
}

int dba_disable_syscall_trace( DBA_TID tid ) {
    return toggle_syscall_tracer( tid, false );
}

int dba_enable_taint_analysis( DBA_TID tid, CONTAMINATION_RECORD tag ) {
    return toggle_taint_analysis( tid, tag, true );
}

int dba_disable_taint_analysis( DBA_TID tid ) {
    return toggle_taint_analysis( tid, 0, false );
}

int dba_start_analysis( DBA_TID tid ) {

    // check TID and if a corresponding DBA context
    if( !is_valid_tid(tid) || dba_tasks[tid] == NULL ) {
        dba_errno = DBA_ERR_INVALID_TID;
        return -1;
    }

    // the task must be IDLE to start analysis
    if( dba_tasks[tid]->state != DBA_TASK_IDLE ) {
        dba_errno = DBA_ERR_INVALID_TSTATE;
        return -1;
    }
        
    // validate host/guest sample path
    if( !is_valid_sample(tid) ) {
        dba_errno = DBA_ERR_INVALID_SAMPLE;
        return -1;
    }

    // check agent is ready
    if( !agent_is_ready() ) {
        dba_errno = DBA_ERR_AGENT_NOTREADY;
        return -1;
    }

    // check DIFT is on if taint option is enabled
    if( dba_tasks[tid]->taint.is_enabled && !dift_is_enabled() ) {
        dba_errno = DBA_ERR_DIFT_NOTREADY;
        return -1;
    }
    
    // spawn DBA analysis thread
    if( pthread_create(&dba_tasks[tid]->thread, NULL, dba_main_internal, dba_tasks[tid]) != 0 ) {
        dba_errno = DBA_ERR_FAIL;
        return -1;
    }
 
    return 0;
}
