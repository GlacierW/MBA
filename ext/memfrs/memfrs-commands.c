/*
 *  MBA Virtual Machine Memory Forensic qemu command implementation
 *
 *  Copyright (c)   2012 Chiwei Wang
 *                  2016 Chiawei Wang
 *                  2016 Chongkuan Chen
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
#include "qemu-common.h"
#include "monitor/monitor.h"

#include "exec/cpu-all.h"

#include "ext/memfrs/memfrs-commands.h"
#include "ext/memfrs/memfrs.h"
#include "ext/memfrs/memfrs-priv.h"
#include "ext/memfrs/kernel.h"
#include "ext/memfrs/vad.h"
#include "ext/memfrs/kmod.h"

#include "qmp-commands.h"

/******************************************************************
* PURPOSE : Load the data structure's information into 
            g_struct_info from json file of sdb_path
******************************************************************/
void do_load_structures(Monitor *mon, const QDict *qdict)
{
    const char* sdb_path = qdict_get_str(qdict, "sdb");
    memfrs_load_structs(sdb_path);
}

/******************************************************************
* PURPOSE : List the running process
******************************************************************/
void do_process_list(Monitor *mon, const QDict *qdict)
{
    CPUState *cpu, *thiscpu=NULL;    

    // Find the first CPU
    CPU_FOREACH(cpu)
    {
        monitor_printf(mon, "%p\n", cpu);
        thiscpu = cpu;
        break;
    }

    memfrs_enum_proc_list(g_kpcr_ptr, thiscpu);
}

void do_scan_kernel(Monitor *mon, const QDict *qdict)
{
    CPUState *thiscpu=NULL;
    uint64_t base = 0;

    if( (base = memfrs_get_nt_kernel_base()) != 0)
    {
        monitor_printf(mon, "Kernel already find at %"PRIx64"\n", base); 
        return;
    }
    thiscpu = ENV_GET_CPU((CPUArchState*)mba_mon_get_cpu());
 
    base = memfrs_find_nt_kernel_base(thiscpu);
    if(base != 0)
        monitor_printf(mon, "Kernel found %"PRIx64"\n", base);
    else
        monitor_printf(mon, "Kernel not found\n");
}

// TODO: Fix ir, possible cr3 problem
void do_scan_virmem(Monitor *mon, const QDict *qdict)
{
    uint64_t start_addr = qdict_get_int(qdict, "start");
    uint64_t end_addr = qdict_get_int(qdict, "end");
    const char* pattern = qdict_get_str(qdict, "pattern");
    UT_array *match_addr;
    uint64_t *p;
    CPUState *thiscpu=NULL;


    thiscpu = ENV_GET_CPU((CPUArchState*)mba_mon_get_cpu());
    match_addr = memfrs_scan_virmem(thiscpu, start_addr, end_addr, pattern, strlen(pattern));

    printf("%p\n", match_addr);
    for(p=(uint64_t*)utarray_front(match_addr); p!=NULL; p=(uint64_t*)utarray_next(match_addr,p))
    {
        printf("%"PRIx64"\n",*p);
    }
    
}

void do_scan_phymem(Monitor *mon, const QDict *qdict)
{
    uint64_t start_addr = qdict_get_int(qdict, "start");
    uint64_t end_addr = qdict_get_int(qdict, "end");
    const char* pattern = qdict_get_str(qdict, "pattern");
    uint64_t *p;
    UT_array *match_addr;

    match_addr = memfrs_scan_phymem(start_addr, end_addr, pattern, strlen(pattern));   

    for(p=(uint64_t*)utarray_front(match_addr); p!=NULL; p=(uint64_t*)utarray_next(match_addr,p))
    {
        printf("%"PRIx64"\n",*p);
    }

}

void do_show_phymem_content(Monitor *mon, const QDict *qdict)
{
    uint64_t target_addr = qdict_get_int(qdict, "addr");
    uint64_t target_length = qdict_get_int(qdict, "len");
    //int i;
    uint8_t* buf = (uint8_t*)malloc(target_length);
    if(buf == NULL){
        monitor_printf(mon, "Cannot allocate memory for do_show_memory_taint_map()\n");
        return;
    }

    monitor_printf(mon, "Display memory content %"PRIx64" to %"PRIx64"\n", target_addr, target_addr+target_length);
    cpu_physical_memory_read(target_addr, buf, target_length);
    hexdump(mon, buf, target_length);
    free(buf);
}

void do_show_virmem_content(Monitor *mon, const QDict *qdict)
{
    uint64_t target_cr3	= 0;
    uint64_t target_addr = qdict_get_int(qdict, "addr");
    uint64_t target_length = qdict_get_int(qdict, "len");
    uint8_t* buf;

    CPUState *cpu=NULL;

    if(qdict_haskey(qdict, "cr3"))
        target_cr3 = qdict_get_int(qdict, "cr3");

    cpu = ENV_GET_CPU((CPUArchState*)mba_mon_get_cpu());
    
    buf = (uint8_t*)malloc(target_length);
    if(buf == NULL){
        monitor_printf(mon, "Cannot allocate memory for do_show_memory_taint_map()\n");
        return;
    }
    
    memset(buf, 0x00, target_length);
  
    memfrs_get_virmem_content( cpu, target_cr3, target_addr, target_length, buf);
    hexdump(mon, buf, target_length);
    free(buf);
}


void do_traverse_vad(Monitor *mon, const QDict *qdict)
{
    CPUState *cpu, *thiscpu=NULL;
    uint64_t eprocess_ptr = qdict_get_int(qdict, "eprocess_addr");
    UT_array *vad_list;
    vad_node* p;

    monitor_printf(mon, "vad parse\n");

    // Find the first CPU
    CPU_FOREACH(cpu)
    {
        monitor_printf(mon, "%p\n", cpu);
        thiscpu = cpu;
        break;
    }

    vad_list = memfrs_traverse_vad_tree(eprocess_ptr ,  thiscpu);
    for(p=(vad_node*)utarray_front(vad_list); p!=NULL; p=(vad_node*)utarray_next(vad_list,p))
    {
        printf("%"PRIx64" -- %"PRIx64" %s\n", p->start_viraddr, p->end_viraddr, p->filename);
        //printf("%"PRIx64"\n",p->end_viraddr);
        //printf("%s")
    }
}

#define MAXMEM 0x80000000
#define POOL_TAG_MODULE "MmLd"
#define HARDCODE_POOL_HEADER_SIZE 0x10
#define SIZEOFUNICODESTRING 0x10

void do_module_list(Monitor *mon, const QDict *qdict)
{
    CPUState *cpu=NULL;
    cpu = ENV_GET_CPU((CPUArchState*)mba_mon_get_cpu());

    memfrs_scan_module(cpu); 
}

/******************************************************************
* PURPOSE : Load the global variable's(symbols) information into 
            g_globalvar_info from json file of gvar_db
******************************************************************/
void do_load_global_variable(Monitor *mon, const QDict *qdict)
{
    const char* gvar_path = qdict_get_str(qdict, "gvar_db");
    memfrs_load_globalvar(gvar_path);
}

/******************************************************************
* PURPOSE : Get the virtual memory address of symbols with name gvar  
******************************************************************/
void do_get_gvar_vmem(Monitor *mon, const QDict *qdict)
{
    const char* name = qdict_get_str(qdict, "gvar");
    CPUState *thiscpu=NULL;
    uint64_t base = 0;
    json_object *gvar = NULL;
    if( (base = memfrs_get_nt_kernel_base()) != 0)
    {
        monitor_printf(mon, "Kernel already find at %"PRIx64"\n", base);
        //return;
    } else
    {
        thiscpu = ENV_GET_CPU((CPUArchState*)mba_mon_get_cpu());
        base = memfrs_find_nt_kernel_base(thiscpu);
        if(base != 0)
            monitor_printf(mon, "Kernel found %"PRIx64"\n", base);
        else
        {
            monitor_printf(mon, "Kernel not found\n");
            return;
        }
   }
    
   gvar = memfrs_q_globalvar(name);
   if( gvar != NULL )
       monitor_printf(mon, "%s @ %"PRIx64"\n", name, memfrs_gvar_offset(gvar) + base);
   return; 
}

void do_gvar_lookup(Monitor *mon, const QDict *qdict)
{
    reverse_symbol* sym_rev_hash = NULL;
    uint64_t addr = qdict_get_int(qdict, "addr");
    char* name = NULL;

    printf("Symbol Lookup\n");
    sym_rev_hash = memfrs_build_gvar_lookup_map();
    name = memfrs_get_symbolname_via_address( sym_rev_hash, addr);
    printf("%s\n", name);
    memfrs_free_reverse_lookup_map(sym_rev_hash);
}

/******************************************************************
* PURPOSE : Get the process handles
******************************************************************/
void do_process_handles_list(Monitor *mon, const QDict *qdict)
{

    CPUState *cpu, *thiscpu=NULL;    
    json_object *handles;
    handles = json_object_new_object();

    uint64_t target_cr3	= 0x0;

    if(qdict_haskey(qdict, "target_cr3"))
        target_cr3 = qdict_get_int(qdict, "target_cr3");

    // Find the first CPU
    CPU_FOREACH(cpu)
    {
        monitor_printf(mon, "%p\n", cpu);
        thiscpu = cpu;
        break;
    }

    memfrs_enum_proc_handles( target_cr3, g_kpcr_ptr, thiscpu, handles );

   // printf("\n\n%s\n\n", json_object_to_json_string(handles));
}
