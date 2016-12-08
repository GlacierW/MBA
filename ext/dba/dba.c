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

#include "net/slirp.h"
#include "qmp-commands.h"
#include "include/utarray.h"

dba_context* dba_tasks[DBA_MAX_TASKS] = { 0 };
DBA_ERRNO    dba_errno;

extern QemuMutex qemu_global_mutex;

// next Task ID, round by DBA_MAX_TASKS
static int next_tid = 0;

// Private functions
static int toggle_taint_analysis( dba_context* ctx, CONTAMINATION_RECORD tag, bool is_enable ) {
    
    if( ctx == NULL ) {
        dba_errno = DBA_ERR_FAIL;
        return -1;
    }

    if( tag == 0 ) {
        dba_errno = DBA_ERR_TAINT_TAG;
        return -1;
    }

    ctx->taint.is_enabled = is_enable;
    ctx->taint.tag        = tag;
    return 0;
}

static int toggle_syscall_tracer( dba_context* ctx, bool is_enable ) {

    if( ctx == NULL ) {
        dba_errno = DBA_ERR_FAIL;
        return -1;
    }

    ctx->syscall.is_enabled = is_enable;
    return 0;
}

static int get_available_tid( void ) {
 
    int  avai_tid  = next_tid;
    bool available = false;

    // search for DONE task and reuse the record slot
    if( dba_tasks[avai_tid] != NULL ) {
        do {
            // free the old record for reuse
            if( dba_tasks[avai_tid]->state == DBA_TASK_DONE ) {
                json_object_put( dba_tasks[avai_tid]->result );
                free( dba_tasks[avai_tid] );
                available = true;
                break;
            }
            avai_tid = (avai_tid + 1) % DBA_MAX_TASKS;
        } while( avai_tid != next_tid );

        // no empty slot
        if( !available ) 
            return -1;
    }

    // update the next task ID
    next_tid = (avai_tid + 1) % DBA_MAX_TASKS;

    return avai_tid;
}

// macro for agent action polling
#define AGENT_ACT( x ) \
    while( (aret = x) == AGENT_RET_EBUSY ) { asm volatile("pause"); } \
    while( !agent_is_idle() ) { asm volatile( "pause" ); }
static void* dba_main_internal( void* ctx_arg ) {

    MBA_AGENT_RETURN aret;

    dba_context* ctx = ctx_arg;

    ctx->state = DBA_TASK_BUSY;

    // import sample from host into guest & flush disk cache
    AGENT_ACT( agent_import(ctx->sample_gpath, ctx->sample_hpath) );
    AGENT_ACT( agent_sync() );

    if( ctx->taint.is_enabled ) {
        json_object_object_add( ctx->result, DBA_JSON_KEY_TAINT, json_object_new_object() );
        set_sample_tainted( ctx );
    }

    // invoke sample
    AGENT_ACT( agent_invoke(ctx->sample_gpath) );

    // wait for the timer for sample execution
    sleep( ctx->sample_timer );

    // flush buffered read/write of the guest OS
    AGENT_ACT( agent_sync() );

    if( ctx->taint.is_enabled ) 
        enum_tainted_file( ctx );

    ctx->state = DBA_TASK_DONE;

    return NULL;
}
#undef AGENT_ACT

// Public APIs
dba_context* dba_alloc_context( void ) {
    
    dba_context* ctx = NULL;

    ctx = (dba_context*)malloc( sizeof(dba_context) );
    if( ctx == NULL ) 
        dba_errno = DBA_ERR_FAIL;

    ctx->result = json_object_new_object();
    ctx->state  = DBA_TASK_IDLE;

    return ctx;
}

int dba_free_context( dba_context* ctx ) {

    if( ctx != NULL ) 
        json_object_put( ctx->result );

    return 0;
}

int dba_set_timer( dba_context* ctx, size_t seconds ) {

    if( ctx == NULL ) {
        dba_errno = DBA_ERR_FAIL;
        return -1;
    }

    ctx->sample_timer = seconds;
    return 0;
}

int dba_set_sample( dba_context* ctx, const char* path ) {

    if( ctx == NULL ) {
        dba_errno = DBA_ERR_FAIL;
        return -1;
    }

    if( strlen(path) > DBA_MAX_FILENAME ) {
        dba_errno = DBA_ERR_SAMPLE;
        return -1;
    }

    // copy sample host path
    bzero( ctx->sample_hpath, DBA_MAX_FILENAME + 1 );
    strncpy( ctx->sample_hpath, path, DBA_MAX_FILENAME );

    // setup sample guest path
    bzero( ctx->sample_gpath, DBA_MAX_FILENAME + sizeof(DBA_GUEST_SAMPLE_DIR) );
    sprintf( ctx->sample_gpath, "%s%s", DBA_GUEST_SAMPLE_DIR, basename(ctx->sample_hpath) );

    return 0;
}

int dba_enable_syscall_trace( dba_context* ctx ) {
    return toggle_syscall_tracer( ctx, true );
}

int dba_disable_syscall_trace( dba_context* ctx ) {
    return toggle_syscall_tracer( ctx, false );
}

int dba_enable_taint_analysis( dba_context* ctx, CONTAMINATION_RECORD tag ) {
    return toggle_taint_analysis( ctx, tag, true );
}

int dba_disable_taint_analysis( dba_context* ctx ) {
    return toggle_taint_analysis( ctx, 0, false );
}

int dba_start_analysis( dba_context* ctx ) {

    int task_id;

    // valid context
    if( ctx == NULL ) {
        dba_errno = DBA_ERR_FAIL;
        return -1;
    }

    // valid host/guest sample path
    if( 
        ctx->sample_hpath         == NULL 
     || strlen(ctx->sample_hpath) == 0
     || ctx->sample_gpath         == NULL
     || strlen(ctx->sample_gpath) == 0
     ) {
        dba_errno = DBA_ERR_SAMPLE;
        return -1;
    }

    // check agent is ready
    if( !agent_is_ready() ) {
        dba_errno = DBA_TASKERR_AGENT_NOTREADY;
        return -1;
    }

    // check DIFT is on
    if( ctx->taint.is_enabled && !dift_is_enabled() ) {
        dba_errno = DBA_TASKERR_DIFT_NOTREADY;
        return -1;
    }
    
    task_id = get_available_tid();
    if( task_id == -1 ) {
        dba_errno = DBA_TASKERR_NOTASKSLOT;
        return -1;
    }
    
    // spawn DBA analysis thread
    if( pthread_create(&ctx->thread, NULL, dba_main_internal, ctx) != 0 ) {
        dba_errno = DBA_ERR_FAIL;
        return -1;
    }
 
    ctx->task_id = task_id;
    dba_tasks[task_id] = ctx;

    return ctx->task_id;
}
