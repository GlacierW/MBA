/*
 *  System Call Trace Implementation
 *
 *  Copyright (c)   2016 Hao Li
 *                       Chong-Kuan Chen
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
#include "ext/memfrs/ssdt.h"

#define MAX_NM_SYSTRACE 65536
#define MAX_SZ_SYSTRACE_LABEL 16

#define SUCCEED() do{return 0;}while( 0 )
// XXX: INVALID_HANDLE also -1, implicit mix used
#define FAIL( x ) do{systrace_errno = x; return -1;}while( 0 )


SYSTRACE_ERRNO systrace_errno;

typedef struct systrace_record{
    // The id for the tracer
    int id;

    // Label for user to reconize the systrace
    char label[MAX_SZ_SYSTRACE_LABEL];

    // the number of the system call to trace
    int           syscall_number;

    // the callback, invoked with X86CPU, is_invoked, user_argument
    systrace_cb   callback;

    // target cr3, trace all process if CR3_ALL
    target_ulong  cr3;

    // Specific if the tracer monitor on system call entry
    bool is_entry;

    // user argument for callback
    void         *cb_args;

    // linked list token
    struct systrace_record *next;

} systrace_record;

struct {
    // table of records, NULL if not occupied
    systrace_record *records[MAX_NM_SYSTRACE];

    // the rwlock to prevent race condition
    pthread_rwlock_t rwlock;

    // hash table for the system call context(argument, thread id)
    syscall_context* g_syscall_ctx;

    // The UT array for SSDT information
    UT_array* g_ssdt_info;

    // linked list of system call tracer
    systrace_record *ll_records;

} sys_ctx[1] = {
  {
    .records = {NULL},
    .rwlock = PTHREAD_RWLOCK_INITIALIZER,
    .g_syscall_ctx = NULL,
    .g_ssdt_info = NULL, 
    .ll_records = NULL
  }
};

UT_icd uint64_icd = {sizeof(uint64_t), NULL, NULL, NULL };

// Add a new system call tracer record
static systrace_record *new_record( int id, const char* label, target_ulong cr3, int syscall_number, bool is_entry, systrace_cb callback, void *cb_args ) {
    systrace_record *rec = ( systrace_record* )calloc( 1, sizeof(systrace_record) );
    if( rec ) {
        rec->id = id;
        strncpy( rec->label, label, MAX_SZ_SYSTRACE_LABEL );
        rec->syscall_number = syscall_number;
        rec->callback = callback;
        rec->cr3 = cr3;
        rec->cb_args = cb_args;
        rec->is_entry = is_entry;
    }
    return rec;
}

// Generate the context code for system call/return binding
static syscall_context* gen_syscall_context(int syscall_number, CPUX86State *env){
    X86CPU *x86cpu = ( X86CPU* )x86_env_get_cpu( env );
    UT_array *arg_arr = NULL;
    current_thread* thread;
    uint64_t context_idx;
    syscall_context* syscall_ctx;
    // Query current system call info
    ssdt_list_st* ssdt_elt = (ssdt_list_st*)utarray_eltptr(sys_ctx->g_ssdt_info, syscall_number);

    // Initialize system call argument array
    utarray_new(arg_arr, &uint64_icd);
    
    // Calculate context code
    thread = memfrs_get_current_thread(CPU(x86cpu));
    context_idx = (env->cr[3] << (32-12)) | thread->unique_thread ;
    free(thread);

    // Filling context info
    syscall_ctx = (syscall_context*)malloc(sizeof(syscall_context));
    syscall_ctx->context_key = context_idx;
    syscall_ctx->syscall_number = syscall_number;


    // Retrieve arguments in register
    utarray_push_back(arg_arr, &(x86cpu->env.regs[R_ECX]));
    utarray_push_back(arg_arr, &(x86cpu->env.regs[R_EDX]));
    utarray_push_back(arg_arr, &(x86cpu->env.regs[8]));
    utarray_push_back(arg_arr, &(x86cpu->env.regs[9]));

    // Retrieve arguments in the stack
    if(ssdt_elt!=NULL)
    {
        int i;
        for(i = 0; i < ssdt_elt->argnum_on_stack    ; i++ )
        {
            uint64_t v;  
            memfrs_get_virmem_content(CPU(x86cpu), env->cr[3], x86cpu->env.regs[R_ESP] + 0x28 + i*8, sizeof(uint64_t), (uint8_t*)&v);
            utarray_push_back(arg_arr, &v);
        }
    }

    syscall_ctx->argument_arr = arg_arr;

    return syscall_ctx; 
}

// Remove the system call context after system call return
static int free_context(syscall_context* syscall_ctx)
{
    if(syscall_ctx == NULL)
        return 0;
    utarray_free(syscall_ctx->argument_arr);
    HASH_DEL(sys_ctx->g_syscall_ctx, syscall_ctx);
    free(syscall_ctx);
    return 0;
}

// syscall enter/exit event handler
static void event_callret( CPUX86State *env, bool is_enter ) {
    X86CPU *x86cpu = ( X86CPU* )x86_env_get_cpu( env );
    systrace_record *rec;
    target_ulong syscall_number;
    uint64_t context_idx;
    current_thread* thread;
    syscall_context* syscall_ctx;

    // get system call number from register
    syscall_number = env->regs[R_EAX] & 0xffff;    

    //Check memfrs data structure
    if(!memfrs_check_struct_info() || !memfrs_check_globalvar_info())
        return;

    // Get the SSDT information if it's not existed
    if(sys_ctx->g_ssdt_info == NULL){
        sys_ctx->g_ssdt_info = memfrs_enum_ssdt_list( g_kpcr_ptr , CPU(x86cpu));
    }

    if(is_enter){
        // generate current system context as the identifier
        syscall_ctx = gen_syscall_context(syscall_number, env);

        // Insert the context to the hash
        HASH_ADD_INT(sys_ctx->g_syscall_ctx, context_key, syscall_ctx);
    }else{
        // Calculatet context code as index
        thread = memfrs_get_current_thread(CPU(x86cpu));
        context_idx = (env->cr[3] << (32-12)) | thread->unique_thread ;
        free(thread);

        // Find the responding system call context  
        HASH_FIND_INT(sys_ctx->g_syscall_ctx, &context_idx, syscall_ctx);
    }

    // invoke all callbacks matching cr3 and system call number
    LL_FOREACH( sys_ctx->ll_records, rec ) {
        if( rec->cr3 == CR3_ALL || rec->cr3 == env->cr[3] ) {
            if (is_enter && rec-> is_entry ) {
                if( rec->syscall_number == SYSCALL_ALL || rec->syscall_number == syscall_number )
                {
                    rec->callback( x86cpu, true, rec->cb_args, syscall_ctx );
                    
                }
            }
            else if(!is_enter && syscall_ctx!=NULL && !rec-> is_entry){
                 if( rec->syscall_number == SYSCALL_ALL || rec->syscall_number == syscall_ctx->syscall_number)
                 {
                     rec->callback( x86cpu, false, rec->cb_args, syscall_ctx );
                 }               

            }
        }
    }

    if(!is_enter){
        free_context(syscall_ctx);
    }
}

// Public API for add system call tracer 
extern int systrace_add( const char* label, target_ulong target_cr3, int target_syscall_number, bool is_entry, systrace_cb callback, void* cb_args ) {
    int new_handle;
    systrace_record *rec;

    // get empty slot for new tracer
    for( new_handle = 0; new_handle < MAX_NM_SYSTRACE; new_handle++ )
        if( sys_ctx->records[new_handle] == NULL )
            break;

    if( new_handle == MAX_NM_SYSTRACE )
        FAIL( SYSTRACE_ERR_FULL_TRACE );

    // alloc new record
    rec = new_record( new_handle, label, target_cr3, target_syscall_number, is_entry, callback, cb_args );
    if( rec == NULL )
        FAIL( SYSTRACE_ERR_CREATE_FAIL );

    pthread_rwlock_wrlock( &sys_ctx->rwlock );

    // insert new record into list and occupy a table slot
    LL_APPEND( sys_ctx->ll_records, rec );
    sys_ctx->records[new_handle] = rec;

    pthread_rwlock_unlock( &sys_ctx->rwlock );


    return new_handle;
}

// public API for deleting system call tracer
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

        pthread_rwlock_wrlock( &sys_ctx->rwlock );

        LL_DELETE( sys_ctx->ll_records, rec );

        pthread_rwlock_unlock( &sys_ctx->rwlock );

        sys_ctx->records[systrace_handle] = NULL;
        free( rec );
        SUCCEED();
    }

    // record not in list
    FAIL( SYSTRACE_ERR_INTERNAL_BROKEN );
}

// public API for listing all system call tracer
extern int systrace_list(void) {
    systrace_record *rec;
    systrace_record *tmp;
 
    printf("List all systrace\n");
    LL_FOREACH_SAFE( sys_ctx->ll_records, rec, tmp ) {
        printf("\tID: %d, LABEL: %s, SYSNUM: %d, CR3: %016lx, CB: %p\n", 
              rec->id, rec->label, rec->syscall_number, rec->cr3, rec->callback);
    }
    SUCCEED();
}


// private api for implementation
extern void systrace_on_syscall( CPUX86State *env ) {
    return event_callret( env, true );
}

// private api for implementation
extern void systrace_on_sysret( CPUX86State *env ) {
    return event_callret( env, false );
}

// callback of systrace for dumping general syscall info
void cb_log_syscall_info( X86CPU *x86cpu, bool is_invoke, void *args, const syscall_context* syscall_info ) {
    int errcode = 0;

    if( is_invoke ) {
        uint64_t* p = NULL;
        printf( "syscall #%04lx invoked\n", x86cpu->env.regs[R_EAX] & 0xffff );
        printf( "syscall_info #%04x context code %016lx\n", syscall_info->syscall_number, syscall_info->context_key );
        while( (p=(uint64_t*)utarray_next(syscall_info->argument_arr,p))) 
              printf("%016lx\n", *p);

    } else {
        printf( "syscall return\n");
        printf( "syscall_info #%04x context code %016lx\n", syscall_info->syscall_number, syscall_info->context_key );
    }
   

    if( errcode == -1 ) {
        printf( "syscall hook routine error\n" );
    }
}

