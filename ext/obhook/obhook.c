/*
 *  Out-of-Box Hook implementation
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

#include "obhook.h"

#define MASK_KERN_ADDR 0xffff000000000000

// global variables 
obhook_context obhk_ctx[1];
OBHOOK_ERRNO obhook_errno;
bool obhook_pending_hooks;

/// Private function
/// These function should be invoked only in the scope of this file

/// Search available slot in the index table
/// Return index number as the uid for a new obhook, -1 if no more obhook space
static int get_obhook_uid( void ) {

    int i;
    for( i = 0; i < MAX_NM_OBHOOK; ++i )
        if( obhk_ctx->index_tbl[i] == NULL )
            break;

    return (i == MAX_NM_OBHOOK)? -1 : i;
}

/// Check if the given address is in kernel space
/// NOTE that the check is based on the memory layout of Windows 10 x64 
static inline bool is_kern_addr( target_ulong addr ) {
    return ((addr & MASK_KERN_ADDR) == MASK_KERN_ADDR);
}

/// Enable/Disable a registered hook
/// Return 0 on success, -1 and errno is set otherwise
static int toggle_obhk( int obhook_d, bool enabled ) {

    if( obhk_ctx->index_tbl[obhook_d] == NULL ) {
        obhook_errno = OBHOOK_ERR_INVALID_DESCRIPTOR;
        return -1;
    }

    obhk_ctx->index_tbl[obhook_d]->enabled = enabled;

    return 0;
}

/// Internal implementation of out-of-box hook registration
/// Return 0 on success, -1 and errno is set otherwise.
static int add_obhk_internal( target_ulong cr3, target_ulong addr, const char* label, void*(*cb) (void*) ) {

    obhk_ht_record* ht_rec;
    obhk_ht_record* ht_proc_rec;
    obhk_cb_record* cb_rec;
 
    int new_uid;

    // check if there are avaible space for the new hook
    // if yes, take the uid first
    new_uid = get_obhook_uid();
    if( new_uid == -1 ) {
        obhook_errno = OBHOOK_ERR_FULL_HOOK;
        goto obhk_add_fail;
    }

    // if cr3 is 0, indicating universal hook,
    // check if the given address is in kernel space
    if( cr3 == 0 && !is_kern_addr(addr) ) {
        obhook_errno = OBHOOK_ERR_INVALID_ADDR;
        goto obhk_add_fail;
    }

    // check label 
    if( label != NULL && strlen(label) >= MAX_SZ_OBHOOK_LABEL ) {
        obhook_errno = OBHOOK_ERR_INVALID_LABEL;
        goto obhk_add_fail;
    }

    // check callback pointer
    if( cb == NULL ) {
        obhook_errno = OBHOOK_ERR_INVALID_CALLBACK;
        goto obhk_add_fail;
    }

    // check if any hook has been implanted at the process
    // if not, add the 1-layer hash table record
    HASH_FIND( hh, obhk_ctx->hook_tbl, &cr3, sizeof(target_ulong), ht_rec );
    if( ht_rec == NULL ) {

        // create the 1-layer hash table, indexed by CR3
        ht_rec = (obhk_ht_record*)calloc( 1, sizeof(obhk_ht_record) );
        if( ht_rec == NULL ) {
            obhook_errno = OBHOOK_ERR_FAIL;
            goto obhk_add_fail;
        }

        // setup CR3 index
        ht_rec->cr3 = cr3;

        HASH_ADD( hh, obhk_ctx->hook_tbl, cr3, sizeof(target_ulong), ht_rec );
    }

    // check if any hook been implanted at the same address within a process
    HASH_FIND( hh, ht_rec->proc_obhk_tbl, &addr, sizeof(target_ulong), ht_proc_rec );
    if( ht_proc_rec == NULL ) {

        // create the 2-layer hash table, indexed by user address
        ht_proc_rec = (obhk_ht_record*)calloc( 1, sizeof(obhk_ht_record) );
        if( ht_proc_rec == NULL ) {
            obhook_errno = OBHOOK_ERR_FAIL;
            goto obhk_add_fail;
        }

        ht_proc_rec->addr = addr;
        ht_proc_rec->cr3  = cr3;

        HASH_ADD( hh, ht_rec->proc_obhk_tbl, addr, sizeof(target_ulong), ht_proc_rec );
    }


    // allocate callback 
    cb_rec = (obhk_cb_record*)calloc( 1, sizeof(obhk_cb_record) );
    if( cb_rec == NULL ) {
        obhook_errno = OBHOOK_ERR_FAIL;
        goto obhk_add_fail;
    }

    // initialize the callback record
    cb_rec->ht_rec    = ht_proc_rec;
    cb_rec->uid       = new_uid;
    cb_rec->enabled   = true;
    cb_rec->universal = (cb_rec->ht_rec->cr3 == 0)? true : false;
    cb_rec->cb_func   = cb;
    strncpy( cb_rec->label, label, MAX_SZ_OBHOOK_LABEL );

    // add the callback record to the linked list & index table
    LL_APPEND( cb_rec->ht_rec->cb_list, cb_rec );
    obhk_ctx->index_tbl[cb_rec->uid] = cb_rec;

    // setup the flag indicating there are pending hooks
    // to make codeblock re-translated
    obhook_pending_hooks = true;

    return cb_rec->uid;

obhk_add_fail:
    return -1;

}

/// Internal implementation to get the callback functions of a hook
/// Return pointer to the list of the callbacks on success, NULL if not found
static obhk_cb_record* get_obhk_cb_internal( target_ulong cr3, target_ulong addr ) {

    obhk_ht_record* ht_rec;
    obhk_ht_record* ht_proc_rec;

    HASH_FIND( hh, obhk_ctx->hook_tbl, &cr3, sizeof(target_ulong), ht_rec );
    if( ht_rec == NULL ) 
        goto get_obhk_cb_fail;

    HASH_FIND( hh, ht_rec->proc_obhk_tbl, &addr, sizeof(target_ulong), ht_proc_rec );
    if( ht_proc_rec == NULL ) 
        goto get_obhk_cb_fail;

    return ht_proc_rec->cb_list;

get_obhk_cb_fail:
    obhook_errno = OBHOOK_ERR_FAIL;
    return NULL;
}


/// Public API of Out-of-Box hook
/// Each API function should be named with the prefix 'obhook_'
int obhook_add_process( target_ulong cr3, target_ulong addr, const char* label, void*(*cb) (void*) ) {
    return add_obhk_internal( cr3, addr, label, cb );
}

int obhook_add_universal( target_ulong kern_addr, const char* label, void*(*cb) (void*) ) {
    return add_obhk_internal( 0, kern_addr, label, cb );
}

int obhook_delete( int obhook_d ) {

    obhk_ht_record* ht_proc_rec;
    obhk_ht_record* ht_rec;
    obhk_cb_record* cb_rec;

    // get the hook(callback function) record to be deleted
    cb_rec = obhk_ctx->index_tbl[obhook_d];
    if( cb_rec == NULL ) {
        obhook_errno = OBHOOK_ERR_INVALID_DESCRIPTOR;
        goto obhk_del_fail;
    }

    // acquire the hash table record containing the hook
    ht_rec = cb_rec->ht_rec;

    // remove the registered hook from linked list
    LL_DELETE( ht_rec->cb_list, cb_rec );
    
    // remove the 2-layer hash table record if no hook at an address
    if( ht_rec->cb_list == NULL ) {

        HASH_FIND( hh, obhk_ctx->hook_tbl, &ht_rec->cr3, sizeof(target_ulong), ht_proc_rec );

        // delete & free from hash table
        HASH_DELETE( hh, ht_proc_rec->proc_obhk_tbl, ht_rec );
        free( ht_rec );

        // remove the 1-layer hash table record if no hook within a process
        if( HASH_CNT(hh, ht_proc_rec->proc_obhk_tbl) == 0 ) {
            HASH_DELETE( hh, obhk_ctx->hook_tbl, ht_proc_rec );
            free( ht_proc_rec );
        }
    }

    // remove & free hook record from the fast index table
    obhk_ctx->index_tbl[obhook_d] = NULL;
    free( cb_rec );

    return 0;

obhk_del_fail:
    return -1;
}

int obhook_enable( int obhook_d ) {
    return toggle_obhk( obhook_d, true );
}

int obhook_disable( int obhook_d ) {
    return toggle_obhk( obhook_d, false );
}

obhk_cb_record* obhook_getcbs_univ( target_ulong kern_addr ) {

    if( !is_kern_addr(kern_addr) ) {
        obhook_errno = OBHOOK_ERR_INVALID_ADDR;
        return NULL;
    }
    return get_obhk_cb_internal( 0, kern_addr );
}

obhk_cb_record* obhook_getcbs_proc( target_ulong cr3, target_ulong addr ) {
   return get_obhk_cb_internal( cr3, addr );
}
