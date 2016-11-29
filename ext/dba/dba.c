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

#include "net/slirp.h"
#include "qmp-commands.h"
#include "include/utarray.h"

DBA_ERRNO dba_errno;

extern QemuMutex qemu_global_mutex;


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

static bool config_check_taint( dba_context* ctx ) {

    if( ctx->taint.is_enabled && ctx->taint.tag != 0 )
        return true;
    else
        return false;
}

static bool config_check_syscall( dba_context* ctx ) {
    return (ctx->syscall.is_enabled)? true : false;
}

static const char* get_ide0hd0_disk_image( void ) {

    BlockInfoList* blk_list;
    BlockInfoList* info;

    blk_list = qmp_query_block( NULL );
    for( info = blk_list; info != NULL; info = info->next ) {
        if( info->value->has_inserted ) 
            if( strcmp(info->value->device, "ide0-hd0" ) == 0 )
                return info->value->inserted->file;
    }
    return NULL;
}

// macro for agent action polling
#define AGENT_ACT( x ) \
    while( (aret = x) == AGENT_RET_EBUSY ) { asm volatile("pause"); } \
    while( !agent_is_idle() ) { asm volatile( "pause" ); }

static void* dba_main_internal( void* ctx_arg ) {

    dba_context* ctx = ctx_arg;

    MBA_AGENT_RETURN aret;

    const char* guest_image;
    

    UT_array* fblocks;
    
    TSK_DADDR_T  st_haddr, ed_haddr;
    TSK_DADDR_T* haddr_tuple;

    uint64_t haddr;
    
    char**    p_fname;
    UT_array* fnames;
    

    printf( "\n===== DBA task =====\n" );
    printf( "sample(host):    %s\n",  ctx->sample_hpath );
    printf( "sample(guest):   %s\n",  ctx->sample_gpath );
    printf( "execution timer: %zu\n", ctx->sample_timer );
    printf( "taint analysis:  %s\n",  (ctx->taint.is_enabled)? "enabled" : "disabled" );
    printf( "syscall tracer:  %s\n",  (ctx->syscall.is_enabled)? "enabled" : "disabled" );
    printf( "\n" );

    // import sample from host into guest & flush disk cache
    AGENT_ACT( agent_import(ctx->sample_gpath, ctx->sample_hpath) );
    AGENT_ACT( agent_sync() );

    // get disk image patch for disk forensics
    // XXX: currently we by default think that ide0-hd0 is mounted with system image
    qemu_mutex_lock( &qemu_global_mutex );
    guest_image = get_ide0hd0_disk_image();
    fblocks = tsk_find_haddr_by_filename( guest_image, ctx->sample_gpath );
   
    for( haddr_tuple = (TSK_DADDR_T*)utarray_front(fblocks); 
         haddr_tuple != NULL;
         haddr_tuple = (TSK_DADDR_T*)utarray_next(fblocks, haddr_tuple) ) {

        st_haddr = haddr_tuple[0];
        ed_haddr = haddr_tuple[1];

        fnames = tsk_get_filename_by_haddr( guest_image, st_haddr );
        if( fnames == NULL )
            continue;

        for( p_fname = (char**)utarray_front(fnames);
             p_fname != NULL;
             p_fname = (char**)utarray_next(fnames, p_fname) ) {
            
            if( strcasecmp(*p_fname, ctx->sample_gpath) == 0 ) {
                dift_contaminate_disk_or( st_haddr, ed_haddr - st_haddr + 1, ctx->taint.tag );
                break;
            }
        }
        utarray_free( fnames );
    }
    utarray_free( fblocks );
    qemu_mutex_unlock( &qemu_global_mutex );

    // invoke sample
    AGENT_ACT( agent_invoke(ctx->sample_gpath) );

    // wait for the execution timeout
    sleep( ctx->sample_timer );

    // flush buffered read/write of the guest OS
    AGENT_ACT( agent_sync() );

    if( ctx->taint.is_enabled ) {
        
        // TOOD: replace the hardcoded 4096 block size
        for( haddr = 0; haddr < HD_MAX_SIZE; haddr += 4096 ) {

            // check disk address is tainted
            if( dift_get_disk_dirty(haddr) != ctx->taint.tag )
                continue;

            // check if tainted address corresponds to a file
            fnames = tsk_get_filename_by_haddr( guest_image, haddr );
            if( fnames == NULL )
                continue;

            for( p_fname = (char**)utarray_front(fnames);
                 p_fname != NULL;
                 p_fname = (char**)utarray_next(fnames, p_fname) ) {
                
                printf( "%s\n", *p_fname );
            }
            utarray_free( fnames );
        }
    }

    return NULL;
}
#undef  AGENT_ACT

// Public APIs
dba_context* dba_alloc_context( void ) {
    
    dba_context* ctx = NULL;

    ctx = (dba_context*)malloc( sizeof(dba_context) );
    if( ctx == NULL ) 
        dba_errno = DBA_ERR_FAIL;

    ctx->result = json_object_new_object();

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

    // valid context
    if( ctx == NULL ) {
        dba_errno = DBA_ERR_FAIL;
        return -1;
    }

    // valid sample path
    if( strlen(ctx->sample_hpath) == 0 ) {
        dba_errno = DBA_ERR_SAMPLE;
        return -1;
    }
    
    // valid execution timer
    if( ctx->sample_timer == 0 ) {
        dba_errno = DBA_ERR_TIMER;
        return -1;
    }

    // config check for any executable analysis
    if( 
        config_check_taint( ctx )   == false
    &&  config_check_syscall( ctx ) == false
    ) {
        dba_errno = DBA_ERR_EMPTY_ANALYSIS;
        return -1;
    }

    // check agent is ready
    if( !agent_is_ready() ) {
        dba_errno = DBA_TASKERR_AGENT_NOTREADY;
        return -1;
    }

    // check DIFT is on
    if( !dift_is_enabled() ) {
        dba_errno = DBA_TASKERR_DIFT_NOTREADY;
        return -1;
    }
    
    // spawn DBA analysis thread
    if( pthread_create(&ctx->thread, NULL, dba_main_internal, ctx) != 0 ) {
        dba_errno = DBA_ERR_FAIL;
        return -1;
    }
    
    return 0;
}
