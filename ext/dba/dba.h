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
#include "ext/nettramon/nettramon.h"
#include "monitor/monitor.h"

#define DBA_MAX_TASKS               0x1000
#define DBA_MAX_FILENAME            255
#define DBA_GUEST_SAMPLE_DIR        "/Users/dsns/Desktop/"
#define DBA_MAX_TAINT_PACKET_LENGTH 1024 

#define DBA_JSON_KEY_TAINT          "TAINT"
#define DBA_JSON_KEY_SYSCALL        "SYSCALL"

// DBA Task ID
typedef intptr_t DBA_TID;

enum DBA_ERRNO {
    DBA_ERR_FAIL,
    DBA_ERR_TASK_FULL,
    DBA_ERR_INVALID_TID,
    DBA_ERR_INVALID_TSTATE,
    DBA_ERR_INVALID_SAMPLE,

    DBA_ERR_DIFT_TAG,
    DBA_ERR_DIFT_NOTREADY,
    DBA_ERR_AGENT_NOTREADY
};
typedef enum DBA_ERRNO DBA_ERRNO;

enum DBA_TASK_STATE {
    DBA_TASK_IDLE,
    DBA_TASK_BUSY,
    DBA_TASK_DONE
};
typedef enum DBA_TASK_STATE DBA_TASK_STATE;

struct dba_context {

    int task_id;

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

    DBA_TASK_STATE state;

    Monitor* mon;
};
typedef struct dba_context dba_context;


extern DBA_ERRNO    dba_errno;

/// Allocate a context for a new DBA task
///
/// Return the ID of the newly allocated DBA task on success, 
/// otherwise -1 is returned and the dba_errno is set
extern DBA_TID dba_new_task( void );


/// Delete a DBA task by ID. Note that the task to be deleted
/// must be IDLE or DONE.
///
///     \param  tid     DBA task ID
///
/// Return 0 on success, otherwise -1 is return and dba_errno is set
extern int dba_delete_task( DBA_TID tid );

/// Retrieve the DBA context with the corresponding task ID
/// This API is designated to assist developer to READ the 
/// configuration of a DBA task. It is NOT recommended to 
/// modify or free the content returned
///
///     \param  tid     DBA task ID
///
/// Return a pointer to a constant DBA context object on success,
/// otherwise NULL is returned
extern const dba_context* dba_get_task_context( DBA_TID tid );

/// Set the monitor pointer to let task inform user of finishing sample execution
/// Note that the task must be IDLE to be configurable
///
///     \param  tid         DBA task ID
///     \param  mon         pointer to the QENU monitor
///
/// Return 0 on success, otherwise -1 is returned and the dba_errno is set
extern int dba_set_monitor( DBA_TID tid, Monitor* mon );

/// Set the timer for the sample execution for the DBA task by ID
/// Note that the task must be IDLE to be configurable
///
///     \param  tid         DBA task ID
///     \param  seconds     time to wait(sec) for the sample execution
///
/// Return 0 on success, otherwise -1 is returned and the dba_errno is set
extern int dba_set_timer( DBA_TID tid, size_t seconds );

/// Set the sample to perform DBA analysis for the DBA task by ID
/// Note that the task must be IDLE to be configurable
///
///     \param  tid     DBA task ID
///     \param  path    the host-side sample path for DBA analysis
///
/// Return 0 on success, otherwise -1 is returned and the dba_errno is set
extern int dba_set_sample( DBA_TID tid, const char* path );

/// Enable system call tracer for the DBA task speicified by ID
/// Note that the task must be IDLE to be configurable
///
///     \param  tid     DBA task ID
///
/// Return 0 on success, otherwise -1 is returned and the dba_errno is set
extern int dba_enable_syscall_trace( DBA_TID tid );

/// Disable system call tracer for the DBA task speicified by ID
///
/// Note that the task must be IDLE to be configurable
///     \param  tid     DBA task ID
///
/// Return 0 on success, otherwise -1 is returned and the dba_errno is set
extern int dba_disable_syscall_trace( DBA_TID tid );

/// Enable taint analysis for the DBA task speicified by ID
/// Note that the task must be IDLE to be configurable
///
///     \param  tid     DBA task ID
///     \param  tag     taint tag for the taint analysis
///
/// Return 0 on success, otherwise -1 is returned and the dba_errno is set
extern int dba_enable_taint_analysis( DBA_TID tid, CONTAMINATION_RECORD tag );

/// Disable taint analysis for the DBA task speicified by ID
/// Note that the task must be IDLE to be configurable
///
///     \param  tid     DBA task ID
///
/// Return 0 on success, otherwise -1 is returned and the dba_errno is set
extern int dba_disable_taint_analysis( DBA_TID tid );

/// Start a DBA task specified by the given task ID
/// Note that the task must be IDLE to be ready for the DBA analysis
///
///     \param  tid     DBA task ID
///
/// Return 0 on success, otherwise -1 is returned and the dba_errno is set
extern int dba_start_analysis( DBA_TID tid );

#endif
