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

#if !defined(CONFIG_OBHOOK_TEST)
#include <stdbool.h>
#include "uthash.h"
#include "utlist.h"
#include "cpu.h"
#endif

#define MAX_NM_OBHOOK       65535
#define MAX_SZ_OBHOOK_LABEL 16

enum OBHOOK_ERRNO {
    OBHOOK_ERR_FAIL,
    OBHOOK_ERR_FULL_HOOK,
    OBHOOK_ERR_INVALID_CR3,
    OBHOOK_ERR_INVALID_ADDR,
    OBHOOK_ERR_INVALID_LABEL,
    OBHOOK_ERR_INVALID_CALLBACK,
    OBHOOK_ERR_INVALID_DESCRIPTOR
};
typedef enum OBHOOK_ERRNO OBHOOK_ERRNO;

struct obhk_ht_record;
struct obhk_cb_record;

/// To keep the simplicity of implementation,
/// obkh_ht_recrod as the hash table key is designed to support
/// both per-process and universal hooks
struct obhk_ht_record {

    target_ulong addr;

    // cr3 = 0 is reserved for the universal hook
    target_ulong cr3;

    // used for per-process hook
    struct obhk_ht_record* proc_obhk_tbl;

    // callback routines registered
    struct obhk_cb_record* cb_list;

    // handle for hash table
    UT_hash_handle hh;
};
typedef struct obhk_ht_record obhk_ht_record;

struct obhk_cb_record {

    // a reverse-pointer to the hash table record
    struct obhk_ht_record* ht_rec;

    // unique identifier for each hook
    uint16_t uid;

    bool enabled;
    bool universal;

    // user-friendly label string
    char label[MAX_SZ_OBHOOK_LABEL];

    void* (*cb_func) (void*);

    struct obhk_cb_record* next;
};
typedef struct obhk_cb_record obhk_cb_record; 

struct obhook_context {

    // 2-layers lookup hash table for out-of-box hook
    //   The 1-layer is indexed by process CR3
    //   The 2-layer is indexed by address where the hook implanted at
    //
    // Note that the hash record with CR3=0 indicates 
    // the universal hooks, which are trigger regardless 
    // of processes and the address is in kernel space.
    obhk_ht_record* hook_tbl;

    // the fast index table for queries to registered hook
    obhk_cb_record* index_tbl[MAX_NM_OBHOOK];
};
typedef struct obhook_context obhook_context;

/// Global context for out-of-box hooking
///
/// Instead of directly accessing this global
/// context variable, using the obhook API on
/// demands is suggested.
extern obhook_context obhk_ctx[1];

extern OBHOOK_ERRNO obhook_errno;
extern bool obhook_pending_hooks;

/// Add a process-aware, out-of-box hook at the given address
///
///     \param  cr3     CR3 value of the targeted process, should not be zero
///     \param  addr    address to implant the hook
///     \param  label   user-friendly name for the hook (optional, can be NULL)
///     \param  cb      callback routine to be invoked when the hook is triggered
///                     Note that each callback routine will be invoked and given
///                     a pointer to the current CPU state structure
///
/// Return a new obhook descriptor on success, otherwise -1 is returned and the obhook_errno is set
extern int obhook_add_process( target_ulong cr3, target_ulong addr, const char* label, void*(*cb) (void *) );

/// Add an universal, out-of-box hook at the given kernel-space address
///
/// NOTE that the address should be kernel-level address since 
/// it makes no sense to implant a universal hook at a user-level address, 
/// which is in per-process, individual address space.
///
///     \param  kern_addr   kernel address to implant the hook
///     \param  label       user-friendly name for the hook (optional, can be NULL)
///     \param  cb          callback routine to be invoked when the hook is triggered
///                         Note that each callback routine will be invoked and given
///                         a pointer to the current CPU state structure
///
/// Return a new obhook descriptor on success, otherwise -1 is returned and the obhook_errno is set
extern int obhook_add_universal( target_ulong kern_addr, const char* label, void*(*cb) (void *) );

/// Delete a out-of-box hook by the given descriptor
///
///     \param  obhook_d    decriptor of the hook to be deleted
///
/// Return 0 on success, otherwise -1 is returned and the obhook_errno is set
extern int obhook_delete( int obhook_d );

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

/// Get the callbacks of a universal hook implanted at the specified address
///
///     \param  kern_addr   kernel address where the hook implanted at
///
/// Return a pointer to the list of callbacks of the hook on success, NULL and obhook_errno iset otherwise
extern obhk_cb_record* obhook_getcbs_univ( target_ulong kern_addr );

/// Get the callbacks of a per-process hook implanted at the specified address
///
///     \param  cr3     CR3 value of the targeted process, should not be zero
///     \param  addr    address where the hook implanted at
///
/// Return a pointer to the list of callbacks of the hook on success, NULL and obhook_errno iset otherwise
extern obhk_cb_record* obhook_getcbs_proc( target_ulong cr3, target_ulong addr );

#endif
