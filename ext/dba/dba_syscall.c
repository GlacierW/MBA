#include "dba_syscall.h"

#include <libgen.h>

#include "ext/systrace/systrace.h"
#include "monitor/monitor.h"
#include "utlist.h"

#include "../memfrs/memfrs.h"
#include "../obhook/obhook.h"

#define MAX_SZ_PER_TRACE_LOG 140
#define MAX_SZ_IMG_NAME 16

#define SUCCEED() do{return 0;}while( 0 )
#define FAIL( x ) do{dba_errno = x; return -1;}while( 0 )

#define LOG_ERROR( x ) do {\
        jo_arr = json_object_new_array();\
        json_object_array_add( jo_arr, json_object_new_string( x ) );\
        json_object_object_add( jo_out, RFT_SYSCALL_TRACE, jo_arr );}\
        while( 0 )


// context for tracing target program for one (or all) target syscall
typedef struct task_context{
    // sample program name, supposed to be sized not larger than MAX_SZ_IMG_NAME
    char *img_name;

    // registered systrace handle, keep for cleanup
    int systrace_handle;

    // target syscall number
    int syscall_number;

    // callback, will be registered into systrace
    systrace_cb callback;

    // output json array, will be registered into systrace
    json_object *jo_arr_out;

    // DBA task context, keep as id for cleanup of one given DBA task
    dba_context *parent_dba_task;

    struct task_context *next;
} task_context;

// context for the dba-syscall sub-module
static struct {
    // obhook descriptor of MmCreatePeb
    int MmCreatePeb_hook;

    bool MmCreatePeb_registered;

    // linked list of all tracing tasks
    task_context *tasks;
} dba_syscall_context[1];

// supposed to be the only mean to create task_context
static task_context *new_task( char const *program_name, int syscall_number, systrace_cb callback, json_object *jo_arr_out, dba_context *parent_dba_task ) {
    task_context *ctx = ( task_context* )calloc( 1, sizeof(task_context) );
    if( ctx == NULL )
        return NULL;

    ctx->img_name = strndup( program_name, MAX_SZ_IMG_NAME );
    ctx->syscall_number = syscall_number;
    ctx->callback = callback;
    ctx->jo_arr_out = jo_arr_out;
    ctx->parent_dba_task = parent_dba_task;

    return ctx;
}

// callback of systrace for dumping NtCreateFile info
static void cb_dump_NtCreateFile( X86CPU *x86cpu, bool is_invoke, void *args ) {
    CPUState *cpu = ( CPUState* )x86cpu;
    uint16_t string_length;
    target_ulong addr_string;
    uint8_t  copy_buffer[ MAX_SZ_PER_TRACE_LOG * 2 ];
    char     buffer[ MAX_SZ_PER_TRACE_LOG ];
    uint16_t i, j;
    int      errcode;

    json_object   *jo_out = ( json_object* )args;
    target_ulong   cr3    = x86cpu->env.cr[3];
    target_ulong   object_attributes_addr = x86cpu->env.regs[8];

    if( is_invoke ) {
        // get length of filename
        errcode = memfrs_get_mem_struct_content( cpu, cr3, (uint8_t*)&string_length, 2, 
                object_attributes_addr, false, "_OBJECT_ATTRIBUTES", 2, "*ObjectName", "#Length" );
        if( errcode == -1 ) {
            json_object_array_add( jo_out, json_object_new_string( "error: reading string len" ) );
            return;
        }

        // get buffer of filename
        errcode = memfrs_get_mem_struct_content( cpu, cr3, ( uint8_t*)&addr_string, 8,
                object_attributes_addr, false, "_OBJECT_ATTRIBUTES", 2, "*ObjectName", "#Buffer" );
        if( errcode == -1 ) {
            json_object_array_add( jo_out, json_object_new_string( "error: reading string addr" ) );
            return;
        }

        // limit length of string to copy
        if( string_length > MAX_SZ_PER_TRACE_LOG * 2 )
            string_length = MAX_SZ_PER_TRACE_LOG * 2;

        // read buffer to 'copy_buffer'
        errcode = memfrs_get_virmem_content( cpu, cr3, addr_string, string_length, copy_buffer );
        if( errcode == -1 ) {
            json_object_array_add( jo_out, json_object_new_string( "error: reading string" ) );
            return;
        }

        // copy printable string from 'copy_buffer' to 'buffer'
        buffer[MAX_SZ_PER_TRACE_LOG - 1] = '\0';
        snprintf( buffer, MAX_SZ_PER_TRACE_LOG - 1, "File Name: " );
        j = strlen( buffer );
        for( i = 0; i < string_length && j < MAX_SZ_PER_TRACE_LOG - 1; i++ ) {
            if( isprint( copy_buffer[i] ) ) {
                buffer[j] = copy_buffer[i];
                buffer[j + 1] = '\0';
                j++;
            }
        }

        // store file name to result

        json_object_array_add( jo_out, json_object_new_string( buffer ) );
    }
}

// callback of systrace for dumping general syscall info
static void cb_log_syscall_info( X86CPU *x86cpu, bool is_invoke, void *args ) {
    int errcode = 0;
    json_object* jo_out = ( json_object* )args;
    json_object* jo_arr;
    char buffer[MAX_SZ_PER_TRACE_LOG];
    
    // write invocation log to 'jo_out'
    if( is_invoke ) {
        buffer[0] = '\0';
        sprintf( buffer, "syscall #%04lx invoked", x86cpu->env.regs[R_EAX] & 0xffff );
        json_object_array_add( jo_out, json_object_new_string( buffer ) );
    }

    if( errcode == -1 ) {
        LOG_ERROR( "syscall hook routine error" );
    }
}

// callback for hooking all syscalls at the point that target program runs
static void *cb_hook_syscall_for_under_creation_process( void *cpu ) {
    X86CPU *x86cpu = ( X86CPU* )cpu;
    target_ulong eprocess_addr;
    char img_name[MAX_SZ_IMG_NAME];
    task_context *task;
    target_ulong cr3;

    // check if process name correct
    eprocess_addr = x86cpu->env.regs[R_ECX];

    // get program name (16 bytes)
    if( memfrs_get_mem_struct_content(
            (CPUState*)x86cpu, 0, (uint8_t*)img_name, MAX_SZ_IMG_NAME,
            eprocess_addr, false, "_EPROCESS", 1, "#ImageFileName") == -1 ) {

        return NULL;
    }

    // get directory table base (CR3)
    while( memfrs_get_mem_struct_content(
            (CPUState*)x86cpu, 0, (uint8_t*)&cr3, sizeof(cr3), eprocess_addr,
            false, "_EPROCESS", 2, "#Pcb", "#DirectoryTableBase") == -1 ) {

        return NULL;
    }

    // TODO: use mutex lock for 'tasks' list
    LL_FOREACH( dba_syscall_context->tasks, task ) {
        if( strncmp(task->img_name, img_name, MAX_SZ_IMG_NAME) != 0 )
            continue;

        // register syscall trace for all syscall for process 'cr3'
        task->systrace_handle = systrace_add( cr3, task->syscall_number, task->callback, task->jo_arr_out );
        if( task->systrace_handle == INVALID_HANDLE ) {
            // TODO: generate specific error msg here
            json_object_array_add( task->jo_arr_out, json_object_new_string("failed to add systrace callback at MmCreatePeb called") );
        }
    }

    return NULL;
}

// get kernel base, find it if it's not found yet
static target_ulong get_kernel_address_base( void ) {
    target_ulong kernel_base_address;
    CPUState *cpu;

    kernel_base_address = memfrs_get_nt_kernel_base();
    if( kernel_base_address != 0 )
        return kernel_base_address;

    // XXX: why first_cpu here?
    cpu = first_cpu;
    if( cpu == NULL ) {
        return 0;
    }

    return memfrs_find_nt_kernel_base( cpu );
}

// get kernel variable in-guest virtual address
static int get_gvar_addr( const char *var_name, target_ulong *out ) {
    json_object *gvar;
    target_ulong kernel_base_address;

    kernel_base_address = get_kernel_address_base();
    if( kernel_base_address == 0 )
        return -1;

    gvar = memfrs_q_globalvar( var_name );
    if( gvar == NULL )
        return -1;

    *out = memfrs_gvar_offset( gvar ) + kernel_base_address;
    return 0;
}

// allocate output json object, register MmCreatePeb obhook
extern void init_syscall_analysis( dba_context *ctx ) {
    target_ulong mmcreatepeb;
    json_object *jo_arr;
    json_object *jo_out;
    int result;

    // get JSON object for syscall result
    json_object_object_get_ex( ctx->result, DBA_JSON_KEY_SYSCALL, &jo_out );

    // get MmCreatePeb function address
    result = get_gvar_addr( "MmCreatePeb", &mmcreatepeb );
    if( result == -1 ) {
        LOG_ERROR( "failed to hook process creation" );
        return;
    }

    // register MmCreatePeb obhook, for process creation hooking
    if( dba_syscall_context->MmCreatePeb_registered == false ) {
        result = obhook_add_universal( mmcreatepeb, "proc creation", cb_hook_syscall_for_under_creation_process );
        if( result == -1 ) {
            LOG_ERROR( "failed to hook process creation" );
            return;
        }

        dba_syscall_context->MmCreatePeb_registered = true;
        dba_syscall_context->MmCreatePeb_hook = result;
    }

    // create new dba syscall task, add array JSON object to store syscall trace
    jo_arr = json_object_new_array();
    json_object_object_add( jo_out, RFT_SYSCALL_TRACE, jo_arr );
    LL_APPEND( dba_syscall_context->tasks, new_task(basename(ctx->sample_hpath), SYSCALL_ALL, cb_log_syscall_info, (void*)jo_arr, ctx) );

    // create new dba syscall task, add array JSON object to store NtCreateFile info
    jo_arr = json_object_new_array();
    json_object_object_add( jo_out, RFT_NTCREATEFILE, jo_arr );
    LL_APPEND( dba_syscall_context->tasks, new_task(basename(ctx->sample_hpath), 0x55, cb_dump_NtCreateFile, (void*)jo_arr, ctx) );

    // create new dba syscall task, add array JSON object to store NtCreateKey info
    jo_arr = json_object_new_array();
    json_object_object_add( jo_out, RFT_NTCREATEKEY, jo_arr );
    LL_APPEND( dba_syscall_context->tasks, new_task(basename(ctx->sample_hpath), 0x1d, cb_dump_NtCreateFile, (void*)jo_arr, ctx) );

    // create new dba syscall task, add array JSON object to store NtOpenKey info
    jo_arr = json_object_new_array();
    json_object_object_add( jo_out, RFT_NTOPENKEY, jo_arr );
    LL_APPEND( dba_syscall_context->tasks, new_task(basename(ctx->sample_hpath), 0x12, cb_dump_NtCreateFile, (void*)jo_arr, ctx) );

    // create new dba syscall task, add array JSON object to store NtOpenKey info
    jo_arr = json_object_new_array();
    json_object_object_add( jo_out, RFT_NTOPENKEYEX, jo_arr );
    LL_APPEND( dba_syscall_context->tasks, new_task(basename(ctx->sample_hpath), 0x10c, cb_dump_NtCreateFile, (void*)jo_arr, ctx) );

    return;
}

// clean up for the ended dba task
extern void clean_syscall_analysis( dba_context *ctx ) {
    task_context *task;
    task_context *tmp;
    // TODO: use mutex lock for 'tasks' list
    LL_FOREACH_SAFE( dba_syscall_context->tasks, task, tmp ) {
        if( task->parent_dba_task != ctx )
            continue;

        // remove task from list and free content
        LL_DELETE( dba_syscall_context->tasks, task );

        // release resource of tracing task
        systrace_delete( task->systrace_handle );
        //TODO: check if systrace_delete failed and generate specific error msg
        free( task->img_name );
        free( task );
    }
}
