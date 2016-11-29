/*
 *  MBA Dynamic Behavior Analyzer, DBA header
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

#ifndef __DBA_H__
#define __DBA_H__

#include <pthread.h>

#include "json-c/json.h"
#include "ext/tsk/tsk.h"
#include "ext/dift/dift.h"
#include "ext/agent/agent.h"

#define DBA_MAX_FILENAME     255
#define DBA_GUEST_SAMPLE_DIR "/Users/dsns/Desktop/" 

enum DBA_ERRNO {
    DBA_ERR_FAIL,
    DBA_ERR_TIMER,
    DBA_ERR_SAMPLE,
    DBA_ERR_TAINT_TAG,
    DBA_ERR_EMPTY_ANALYSIS,

    DBA_TASKERR_DIFT_NOTREADY,
    DBA_TASKERR_AGENT_NOTREADY
};
typedef enum DBA_ERRNO DBA_ERRNO;

struct dba_context {

    struct {
        bool is_enabled;
        CONTAMINATION_RECORD tag;
    } taint;

    struct {
        bool is_enabled;
    } syscall;

    char   sample_hpath[ DBA_MAX_FILENAME + 1 ];
    char   sample_gpath[ DBA_MAX_FILENAME + sizeof(DBA_GUEST_SAMPLE_DIR) ];
    size_t sample_timer;

    json_object* result;

    pthread_t thread;
};
typedef struct dba_context dba_context;


extern DBA_ERRNO dba_errno;

extern dba_context* dba_alloc_context( void );
extern int          dba_free_context( dba_context* ctx );

extern int dba_set_timer( dba_context* ctx, size_t seconds );
extern int dba_set_sample( dba_context* ctx, const char* path );

extern int dba_enable_syscall_trace( dba_context* ctx );
extern int dba_disable_syscall_trace( dba_context* ctx );

extern int dba_enable_taint_analysis( dba_context* ctx, CONTAMINATION_RECORD tag );
extern int dba_disable_taint_analysis( dba_context* ctx );

extern int dba_start_analysis( dba_context* ctx );

#endif
