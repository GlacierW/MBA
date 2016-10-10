/*
 *  System Call Trace implementation
 *
 *  Copyright (c)   2016 Hao Li
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
#ifndef __SYSTRACE_H__
#define __SYSTRACE_H__
#include "cpu.h"

#define MAX_SERVICE_COUNT 441

// system call numbers
enum {
    SYSCALL_ALL = -1,
};

// constants for special cr3 value
enum {
    CR3_ALL
};

// constants for special handle value
enum {
    INVALID_HANDLE = -1
};
typedef enum {
    SYSTRACE_ERR_FAIL,
    SYSTRACE_ERR_FULL_TRACE,
    SYSTRACE_ERR_CREATE_FAIL,
    SYSTRACE_ERR_INVALID_HANDLE,
    SYSTRACE_ERR_INTERNAL_BROKEN,
} SYSTRACE_ERRNO;
extern SYSTRACE_ERRNO systrace_errno;

// callback type 'systrace_cb' declaration
typedef void ( *systrace_cb )( X86CPU*, bool is_invoke, void* args );

// module APIs

/// Add a system call trace, either process aware or not,
/// for one specific or all system calls
///
///     \param  cr3      CR3 value of the targeted process, CR3_ALL for universal trace
///     \param  syscall  address to implant the trace, SYSCALL_ALL for all system calls
///     \param  callback callback routine to be invoked when the given system call is called
///     \param  cb_args  argument for the given callback routine, simply copy by value
///
/// Return a new systrace descriptor on success, otherwise -1 is returned and the systrace_errno is set
extern int systrace_add( target_ulong cr3, int syscall, systrace_cb callback, void *cb_args );

/// Delete a system call trace, given handle from systrace_add
///
///     \param  systrace_handle handle that represents the deleting trace
///
/// Return 0 on success, otherwise -1 is returned and the systrace_errno is set
extern int systrace_delete( int systrace_handle );

#endif
