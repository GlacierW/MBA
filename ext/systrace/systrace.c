/*
 *  System Call Trace header
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
#include "systrace.h"
#include "systrace_priv.h"

#include "cpu.h"
#include "utlist.h"
#include "utarray.h"
#include "ext/obhook/obhook.h"

#define MAX_NM_SYSTRACE 65536

#define SUCCEED() do{return 0;}while( 0 )
// XXX: INVALID_HANDLE also -1, implicit mix used
#define FAIL( x ) do{systrace_errno = x; return -1;}while( 0 )

SYSTRACE_ERRNO systrace_errno;

typedef struct systrace_record{
    // the number of the system call to trace
    int           syscall_number;

    // the callback, invoked with X86CPU, is_invoked, user_argument
    systrace_cb   callback;

    // target cr3, trace all process if CR3_ALL
    target_ulong  cr3;

    // user argument for callback
    void         *cb_args;

    // linked list token
    struct systrace_record *next;

} systrace_record;

struct {
    // table of records, NULL if not occupied
    systrace_record *records[MAX_NM_SYSTRACE];

    // linked list of records
    systrace_record *ll_records;

} sys_ctx[1];

static systrace_record *new_record( target_ulong cr3, int syscall_number, systrace_cb callback, void *cb_args ) {
    systrace_record *rec = ( systrace_record* )calloc( 1, sizeof(systrace_record) );
    if( rec ) {
        rec->syscall_number = syscall_number;
        rec->callback = callback;
        rec->cr3 = cr3;
        rec->cb_args = cb_args;
    }
    return rec;
}

// syscall enter/exit event handler
static void event_callret( CPUX86State *env, bool is_enter ) {
    X86CPU *x86cpu = ( X86CPU* )x86_env_get_cpu( env );
    systrace_record *rec;
    target_ulong syscall_number;
    
    // get system call number from register
    syscall_number = env->regs[R_EAX] & 0xffff;
    
    // invoke all callbacks matching cr3 and system call number
    LL_FOREACH( sys_ctx->ll_records, rec ) {
        if( rec->cr3 == CR3_ALL || rec->cr3 == env->cr[3] ) {
            if (is_enter) {
                if( rec->syscall_number == SYSCALL_ALL || rec->syscall_number == syscall_number )
                    rec->callback( x86cpu, true, rec->cb_args );
            }
            else {
                // TODO: tell syscall number and call rec->callback
            }
        }
    }
}

extern int systrace_add( target_ulong target_cr3, int target_syscall_number, systrace_cb callback, void* cb_args ) {
    int new_handle;
    systrace_record *rec;

    // get empty slot for new tracer
    for( new_handle = 0; new_handle < MAX_NM_SYSTRACE; new_handle++ )
        if( sys_ctx->records[new_handle] == NULL )
            break;

    if( new_handle == MAX_NM_SYSTRACE )
        FAIL( SYSTRACE_ERR_FULL_TRACE );

    // alloc new record
    rec = new_record( target_cr3, target_syscall_number, callback, cb_args );
    if( rec == NULL )
        FAIL( SYSTRACE_ERR_CREATE_FAIL );

    // insert new record into list and occupy a table slot
    LL_APPEND( sys_ctx->ll_records, rec );
    sys_ctx->records[new_handle] = rec;

    return new_handle;
}

extern int systrace_delete( int systrace_handle ) {
    systrace_record *rec;
    systrace_record *tmp;
    systrace_record *target_rec;
    
    // get corresponding record of given handle
    target_rec = sys_ctx->records[systrace_handle];

    if( target_rec == NULL ) {
        FAIL( SYSTRACE_ERR_INVALID_HANDLE );
    }

    // remove record from list, freeing the record and its content
    LL_FOREACH_SAFE( sys_ctx->ll_records, rec, tmp ) {
        if( rec != target_rec )
            continue;

        LL_DELETE( sys_ctx->ll_records, rec );

        sys_ctx->records[systrace_handle] = NULL;
        free( rec );
        SUCCEED();
    }

    // record not in list
    FAIL( SYSTRACE_ERR_INTERNAL_BROKEN );
}

// private api for implementation
extern void systrace_on_syscall( CPUX86State *env ) {
    return event_callret( env, true );
}

// private api for implementation
extern void systrace_on_sysret( CPUX86State *env ) {
    return event_callret( env, false );
}
