/*
 *  Tracer abilities of DBA
 *
 *  Copyright (c)    2017 JuiChien Jao
 *
 * This library is free software you can redistribute it and/or
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
#include "dba_tracer.h"
#include "ext/obhook/obhook.h"
#include "disas/disas.h"

// Tag in DBA result
#define RFT_INSTR "Instruction"
#define RFT_BLOCK "Block"

/// Callback Function for instruction tracer
/// Decode the instructions, write into a buffer via open_memstream, and append to the result.
///
/// Parameters are defined by "ext/tracer/tracer.h"
///
/// Return None
static void* cb_instr_tracer( void* env_state, uint64_t start_addr, uint64_t useless_arg, void* cur_ctx ) {

    X86CPU* cpu = X86_CPU(env_state);
    CPUX86State *env = &cpu->env;
    dba_context* ctx = (dba_context*)cur_ctx;
    char*   instr_buf;
    size_t buf_size;
    FILE* instr_fp = open_memstream( &instr_buf, &buf_size );

    json_object*    jo_tracer;
    json_object*    jo_instr;
   
    target_disas_inst_count( instr_fp, env, start_addr, 100, 1, 2);
    fprintf( instr_fp, "\teax %08lx\t", env->regs[R_EAX]);
    fprintf( instr_fp, "ebx %08lx\t", env->regs[R_EBX]);
    fprintf( instr_fp, "ecx %08lx\t", env->regs[R_ECX]);
    fprintf( instr_fp, "edx %08lx\n", env->regs[R_EDX]);
    fprintf( instr_fp, "\tcr3 %08lx\n", env->cr[3]);
    fprintf( instr_fp, "\n");
    fclose( instr_fp );

    // get JSON object for tracer
    json_object_object_get_ex( ((dba_context*)ctx)->result, DBA_JSON_KEY_TRACER, &jo_tracer); 
   
    // check if the JSON object is already in the tracer result
    if ( !json_object_object_get_ex( jo_tracer, RFT_INSTR, &jo_instr ) ) {
        // add array JSON object to store tainted packet
        jo_instr = json_object_new_array();
        json_object_object_add( jo_tracer, RFT_INSTR, jo_instr );
    }

    // Add result to result
    json_object_array_add( jo_instr, json_object_new_string( instr_buf ) );
    
    // This "free" operation must be done, must be done, must be done...,
    // since the open_memstream will not handle for this allocated buffer.
    free( instr_buf );

    return NULL;
}

/// Callback Function for block tracer
/// Decode the block instructions, write into a buffer via open_memstream, and append to the result.
///
/// Parameters are defined by "ext/tracer/tracer.h"
///
/// Return None
static void* cb_block_tracer( void* env_state, uint64_t start_addr, uint64_t end_addr, void* cur_ctx ) {
    
    X86CPU* cpu = X86_CPU(env_state);
    CPUX86State *env = &cpu->env;
    dba_context* ctx = (dba_context*)cur_ctx;

    char*   instr_buf;
    size_t buf_size;
    FILE* instr_fp = open_memstream( &instr_buf, &buf_size );
    uint64_t tmp_addr = start_addr;

    json_object*    jo_tracer;
    json_object*    jo_block;
    
    while ( tmp_addr <= end_addr ) {
        tmp_addr += target_disas_inst_count( instr_fp, env, tmp_addr, 100, 1, 2);
        fprintf( instr_fp, "\teax %08lx\t", env->regs[R_EAX]);
        fprintf( instr_fp, "ebx %08lx\t", env->regs[R_EBX]);
        fprintf( instr_fp, "ecx %08lx\t", env->regs[R_ECX]);
        fprintf( instr_fp, "edx %08lx\n", env->regs[R_EDX]);
        fprintf( instr_fp, "\tcr3 %08lx\n", env->cr[3]);
        fprintf( instr_fp, "\n");
    }
    fclose(instr_fp);

    // get JSON object for tracer
    json_object_object_get_ex( ((dba_context*)ctx)->result, DBA_JSON_KEY_TRACER, &jo_tracer);
   
    // check if the JSON object is already in the tracer result
    if ( !json_object_object_get_ex( jo_tracer, RFT_BLOCK, &jo_block ) ) {
        // add array JSON object to store tainted packet
        jo_block = json_object_new_array();
        json_object_object_add( jo_tracer, RFT_BLOCK, jo_block );
    }
    
    json_object_array_add( jo_block, json_object_new_string( instr_buf ) );
    
    free( instr_buf );
    
    return NULL;
}

/// Callback Function for hook on MmCreatePeb
/// Try to get the cr3 of the sample by compareing the program name of the process with the sample name ( Only the begining 14 characters )
///
/// Parameters are defined by "ext/obhook/obhook.h"
///
/// Return None
static void* cb_hook_mmcreatepeb( void* cpu, void* cur_ctx ) {

    X86CPU *x86cpu = ( X86CPU* )cpu;
    dba_context* ctx = (dba_context*)cur_ctx;
    target_ulong eprocess_addr;
    char img_name[DBA_MAX_SZ_IMG_NAME];
    target_ulong cr3;

    // check if process name correct
    eprocess_addr = x86cpu->env.regs[R_ECX];

    // get program name (16 bytes)
    if( memfrs_get_mem_struct_content(
            (CPUState*)x86cpu, 0, (uint8_t*)img_name, DBA_MAX_SZ_IMG_NAME,
            eprocess_addr, false, "_EPROCESS", 1, "#ImageFileName") == -1 ) {
        return NULL;
    }

    // check whether the program name is the same as the task record
    if ( strncmp( ctx->sample_name, img_name, DBA_MAX_SZ_IMG_NAME ) != 0 ) {
        return NULL;
    }

    // get directory table base (CR3)
    if ( memfrs_get_mem_struct_content( (CPUState*)x86cpu, 0, (uint8_t*)&cr3, sizeof(cr3), eprocess_addr,
                                        false, "_EPROCESS", 2, "#Pcb", "#DirectoryTableBase") == -1 ) {
        monitor_printf( ctx->mon, "Fail to get CR3 of Task %d\n", ctx->task_id );
        return NULL;
    }

    // get current task's cr3
    ctx->task_cr3 = cr3;
    
    // TODO: Add Tracer Callback function
    if ( ctx->instr_tracer.instr_enabled ) {
        ctx->instr_tracer.instr_tracer_cb_id = tracer_add_inst_tracer( ctx->task_cr3, "DBA_INSTR", ctx->instr_tracer.instr_is_kernel, cb_instr_tracer, cur_ctx );
        if ( ctx->instr_tracer.instr_tracer_cb_id == -1 ) {
            monitor_printf( ctx->mon, "Fail to register instr tracer of Task %d\n", ctx->task_id );
            return NULL;
        }
        if ( tracer_enable_tracer( ctx->instr_tracer.instr_tracer_cb_id ) == -1 ) {
            monitor_printf( ctx->mon, "Fail to enable instr tracer of Task %d\n", ctx->task_id );
            return NULL;
        }
    }

    if ( ctx->instr_tracer.block_enabled ) {
        ctx->instr_tracer.block_tracer_cb_id = tracer_add_block_tracer( ctx->task_cr3, "DBA_BLOCK", ctx->instr_tracer.block_is_kernel, cb_block_tracer, cur_ctx );
        if ( ctx->instr_tracer.block_tracer_cb_id == -1 ) {
            monitor_printf( ctx->mon, "Fail to register block tracer of Task %d\n", ctx->task_id );
            return NULL;
        }
        if ( tracer_enable_tracer( ctx->instr_tracer.block_tracer_cb_id ) == -1 ) {
            monitor_printf( ctx->mon, "Fail to enable block tracer of Task %d\n", ctx->task_id );
            return NULL;
        }
    }

    return NULL;

}

// get kernel base, find it if it's not get yet
target_ulong get_kernel_address_base( void ) {
    target_ulong kernel_base_address;
    CPUState *cpu;

    kernel_base_address = memfrs_get_nt_kernel_base();
    if( kernel_base_address != 0 )
        return kernel_base_address;
    
    // try and error
    cpu = first_cpu;
    if( cpu == NULL ) {
        return 0;
    }

    return memfrs_find_nt_kernel_base( cpu );
}

// get kernel variable in-guest virtual address
int get_gvar_addr( const char *var_name, target_ulong *out ) {
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

// Setup hook on the mmcreatepeb so as to get the needed information of sample
int set_obhook_on_mmcreatepeb( dba_context* ctx ) {

    target_ulong mmcreatepeb;
    json_object *jo_out;
    int ret_val;

    // get JSON object for syscall result
    json_object_object_get_ex( ctx->result, DBA_JSON_KEY_SYSCALL, &jo_out );
    
    // get MmCreatePeb function address
    ret_val = get_gvar_addr( "MmCreatePeb", &mmcreatepeb );
    if( ret_val == -1 ) {
        monitor_printf( ctx->mon, "Task %d failed to get MmCreatePeb function address\n", ctx->task_id );
        return 1;
    }
    
    // register MmCreatePeb obhook, for process creation hooking
    ret_val = obhook_add_universal( mmcreatepeb, DBA_CALL_BACK_TAG, cb_hook_mmcreatepeb, (void*)ctx );
    if( ret_val == -1 ) {
        monitor_printf( ctx->mon, "Task %d failed to hook process creation\n", ctx->task_id );
        return 1;
    }

    // Record the callback function ID.
    ctx->instr_tracer.mmcreatepeb_hook_id = ret_val;

    return 0;
}

