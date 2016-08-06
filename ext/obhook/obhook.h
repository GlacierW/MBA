/*
 *  Out-of-Box Hook header
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
#ifndef __OBHOOK_H__
#define __OBHOOK_H__

#include "cpu.h"

#define MAX_NM_OBHOOK       65535
#define MAX_SZ_OBHOOK_LABEL 16

enum OBHOOK_ERRNO {
    OBHOOK_ERR_FAIL,
    OBHOOK_ERR_FULL_HOOK,
    OBHOOK_ERR_INVALID_ADDR,
    OBHOOK_ERR_INVALID_LABEL,
    OBHOOK_ERR_INVALID_CALLBACK,
    OBHOOK_ERR_INVALID_DESCRIPTOR
};
typedef enum OBHOOK_ERRNO OBHOOK_ERRNO;

extern OBHOOK_ERRNO obhook_errno;

/// Add a process-aware, out-of-box hook toward the given process address.
///
///     \param  cr3     CR3 value of the targeted process
///     \param  addr    process/user address to implant the hook
///     \param  label   user-friendly name for the hook (optional, can be NULL)
///     \param  cb      callback routine to be invoked when the hook is triggered
///                     Note that each callback routine will be invoked and given
///                     a pointer to the current CPU state structure
///
/// Return a new obhook descriptor on success, otherwise -1 is returned and the obhook_errno is set
extern int obhook_add_process( target_ulong cr3, target_ulong user_addr, char* label, void*(*cb) (void *) );

/// Add an universal, out-of-box hook toward the given address
///
/// NOTE that the address should be kernel-level address, 
/// it makes no sense to implant a universal hook at a user-level address, 
/// which is in per-process, individual address space.
///
///     \param  addr    kernel address to implant the hook
///     \param  label   user-friendly name for the hook (optional, can be NULL)
///     \param  cb      callback routine to be invoked when the hook is triggered
///                     Note that each callback routine will be invoked and given
///                     a pointer to the current CPU state structure
///
/// Return a new obhook descriptor on success, otherwise -1 is returned and the obhook_errno is set
extern int obhook_add_universal( target_ulong kern_addr, char* label, void*(*cb) (void *) );

/// Enable a out-of-box hook by the given descriptor
///
///     \param  obhook_d    descriptor of the hook to be enabled
///
/// Return 0 on success, otherwise -1 is returned and the obhook_errno is set
extern int obhook_enable( int obhook_d );

/// Disable a out-of-box hook by the given descriptor
///
///     \param  obhook_d    descriptor of the hook to be disabled
///
/// Return 0 on success, otherwise -1 is returned and the obhook_errno is set
extern int obhook_disable( int obhook_d );

/// Delete a out-of-box hook by the given descriptor
///
///     \param  obhook_d    decriptor of the hook to be deleted
///
/// Return 0 on success, otherwise -1 is returned and the obhook_errno is set
extern int obhook_delete( int obhook_d );

/// List all registered out-of-box hook
///
/// Return 0 on success, -1 otherwise
extern int obhook_list( void );

#endif
