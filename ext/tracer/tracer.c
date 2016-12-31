/*
 *  MBA Tracer implementation
 *
 *  Copyright (c)   2016 Chiawei Wang
 *                  2016 ChongKuan Chen
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
#include <stdio.h>
#include <stdbool.h>

#include "tracer.h"
#include "utlist.h"
#include "qemu-common.h"
#include "include/exec/cpu-common.h"
#include "exec/cpu-all.h"
#include "disas/disas.h"

#define MASK_KERN_ADDR 0xffff000000000000

int g_tracer_enable = 0;
// Inst Trace Linked-List Head
tracer_cb_record *g_process_tracer_head =NULL;
tracer_cb_record *g_universal_kernel_tracer_head =NULL;
tracer_cb_record *g_universal_tracer_head =NULL;
//tracer_cb_record *g_kernel_inst_head =NULL;



tracer_cb_record *g_block_head =NULL;

/// Check if the given address is in kernel space
/// NOTE that the check is based on the memory layout of Windows 10 x64 
inline bool tracer_is_kern_addr( uint64_t addr ) {
    return ((addr & MASK_KERN_ADDR) == MASK_KERN_ADDR);
}

void tracer_enable_tracer(void)
{
    g_tracer_enable = 1;
    printf("enable tracer\n");
}

void tracer_disable_tracer(void)
{
    g_tracer_enable = 0;
    printf("disable tracer\n");
}

int tracer_is_enable(void)
{
    //if(g_enable){
    //    return 1;
    //}
    //return 0;
    return g_tracer_enable;
}

static void* default_callback(void* env_state, uint64_t pc)
{
    X86CPU *cpu = X86_CPU(env_state);
    CPUX86State *env = &cpu->env;
    //CPUState* env = (CPUX86State*)env_state;
    if(pc> 0x00401000 && pc < 0x004050f0){
        printf("eip %08lx\t", pc);
        printf("eax %08lx\t", env->regs[R_EAX]);
        printf("cr3 %08lx\n", env->cr[3]);
        target_disas_inst_count( stdout, env, pc, 100, 1, 2);    
    }
    return NULL;
}

static int add_tracer_internal( target_ulong cr3, const char* label, void*(*cb) (void*, uint64_t) ) {

    tracer_cb_record* tracer_rec = (tracer_cb_record *) calloc(1, sizeof(tracer_cb_record));
    //Add management for uid
    tracer_rec-> uid = 1;
    strncpy( tracer_rec->label, label, MAX_SZ_TRACER_LABEL );
    //tracer_rec->enabled = true;
    tracer_rec->cr3 = cr3;
    tracer_rec->is_kernel_trace = false;
    tracer_rec->trace_granularity = TRACER_GRANULARITY_INSTR;
    tracer_rec->is_universal = (tracer_rec->cr3 == 0)? true : false;
    tracer_rec->cb_func = cb ;
    
    if(tracer_rec->trace_granularity == TRACER_GRANULARITY_INSTR)
    {
        LL_APPEND(g_process_tracer_head, tracer_rec);
    }

    tracer_rec->enabled = true;

    printf("add new inst tracer\n");
    return 0; 
}


int tracer_add_process( target_ulong cr3, const char* label, void*(*cb) (void*, uint64_t) ) {
    if(cb == NULL) 
        add_tracer_internal(cr3, label, &default_callback);
    else
        add_tracer_internal(cr3, label, cb);
    return 0;
}

int tracer_add_universal( const char* label, void*(*cb) (void*, uint64_t) ) {
    if(cb == NULL)
        add_tracer_internal(0, label, &default_callback);
    else
        add_tracer_internal(0, label, cb);
    return 0;
}

void tracer_list_callback(void){
    tracer_cb_record* rec = NULL;
    LL_FOREACH(g_process_tracer_head, rec)
    {
        printf("cr3: %lx\n", rec->cr3);
        printf("lable: %s\n", rec->label);
        printf("cb: %p\n", rec->cb_func);
    }
}

bool tracer_check_callback( uint64_t pc, uint64_t cr3){
    tracer_cb_record* rec = NULL;
    // If iaddress s in kernel 
    if(tracer_is_kern_addr(pc)){
        //check kernel hook
        LL_FOREACH(g_process_tracer_head, rec)
        {
            if(rec->cr3 == cr3 && rec->is_kernel_trace == true && rec->enabled) return true;
        }
        //if( g_universal_kernel_tracer_head==NULL &&  g_universal_tracer_head==NULL) return false;
        //else 
        return true;        
    
    }else{
        // User level instruction tracer
        LL_FOREACH(g_process_tracer_head, rec)
        {
            if(rec->cr3 == cr3 && rec->enabled) return true;
        }
        //if( g_process_tracer_head==NULL && g_universal_tracer_head==NULL) return false;
        //else 
        return true;
    }  
    //return false;
    //return ( g_process_tracer_head!=NULL|| g_universal_tracer_head!=NULL);
}
