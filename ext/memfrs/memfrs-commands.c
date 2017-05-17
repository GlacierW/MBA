/*
 *  MBA Virtual Machine Memory Forensic qemu command implementation
 *
 *  Copyright (c)   2012 Chiwei Wang
 *                  2016 Chiawei Wang
 *                  2016 Chongkuan Chen
 *                  2016 ELin Ho
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
#include "ext/memfrs/process.h"
#include "ext/memfrs/handles.h"
#include "ext/memfrs/netscan.h"
#include "ext/memfrs/ssdt.h"

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
* PURPOSE : Scan for the kernel base
******************************************************************/
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



/******************************************************************
* PURPOSE : Get windows version
******************************************************************/
void do_get_windows_version(Monitor *mon, const QDict *qdict)
{
    float version;
    CPUState *cpu, *thiscpu=NULL;

    // Find the first CPU
    CPU_FOREACH(cpu)
    {
        monitor_printf(mon, "%p\n", cpu);
        thiscpu = cpu;
        break;
    }

    version = memfrs_get_windows_version(g_kpcr_ptr, thiscpu);
    if( version == -1.0 )
        monitor_printf(mon, "Something is wrong, please check error number\n");
    else if( version == 0.0 )
        monitor_printf(mon, "Unknown Windows version\n");
    else
        monitor_printf(mon, "Windows version number : %.1lf\n", version);
}



/******************************************************************
* PURPOSE : Scan the virmem for the specific pattern
******************************************************************/
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



/******************************************************************
* PURPOSE : Scan the Physical for the specific pattern
******************************************************************/
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



/*****************************************************************************
* PURPOSE : Show the physical mem content starting at <addr> with length <len>
*****************************************************************************/
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



/*****************************************************************************
* PURPOSE : Show the physical mem content starting at <addr> with length <len>
*****************************************************************************/
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



/*********************************************************************************
* PURPOSE : get the physic address of a given virtual address in memory space(cr3)
*********************************************************************************/
void do_get_physic_address(struct Monitor *mon, const struct QDict *qdict)
{
    uint64_t target_cr3     = qdict_get_int(qdict, "cr3");
    uint64_t target_addr    = qdict_get_int(qdict, "addr");
    X86CPU *cpu             = X86_CPU(ENV_GET_CPU((CPUArchState*)mba_mon_get_cpu()));
    hwaddr page             = target_addr & TARGET_PAGE_MASK;

    // XXX(misterlihao@gmail.com):Only one phase copied. Should be fully copied to resist changes.
    X86CPU copied_cpu;
    memcpy(&copied_cpu, cpu, sizeof(copied_cpu));
    copied_cpu.env.cr[3] = target_cr3;

    hwaddr phys_page = cpu_get_phys_page_debug((CPUState*)&copied_cpu, page);
    if (phys_page == -1) {
        monitor_printf(mon, "Cannot find physic page\n");
        return;
    }

    hwaddr phys_addr = phys_page + (target_addr & ~TARGET_PAGE_MASK);
    monitor_printf(mon, "physic address = %p\n", (void*)phys_addr);
}



/******************************************************************
* PURPOSE : Trverse VAD tree
******************************************************************/
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



/******************************************************************
* PURPOSE : scan for the kernel module
******************************************************************/
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



/******************************************************************
* PURPOSE : get the symbolname at specific virtual memory address
******************************************************************/
void do_gvar_lookup(Monitor *mon, const QDict *qdict)
{
    reverse_symbol* sym_rev_hash = NULL;
    uint64_t addr = qdict_get_int(qdict, "addr");
    char* name = NULL;

    printf("Symbol Lookup\n");
    sym_rev_hash = memfrs_build_gvar_lookup_map();
    name = memfrs_get_symbolname_via_address( sym_rev_hash, addr);
    if(name)
        monitor_printf(mon, "%s\n", name);
    memfrs_free_reverse_lookup_map(sym_rev_hash);
}



/*************************************************************************
* PURPOSE : Generate PDB's global var and structure info into profiles_dir
*************************************************************************/
void do_gen_pdb_profiles(Monitor *mon, const QDict *qdict)
{
    CPUState *thiscpu=NULL;
    //const char* pdb = qdict_get_str(qdict, "pdb_path");
    const char* profile_dir = qdict_get_str(qdict, "profiles_dir");
    //void* module_struct = NULL;
    uint64_t base = 0;

    //Check kernel info
    if(memfrs_get_nt_kernel_base() == 0)
    {
        monitor_printf(mon, "No kernel information available, scan the kernel first...\n");

    }
    thiscpu = ENV_GET_CPU((CPUArchState*)mba_mon_get_cpu());

    base = memfrs_find_nt_kernel_base(thiscpu);
    if(base != 0)
        monitor_printf(mon, "Kernel found %"PRIx64"\n", base);
    else
        monitor_printf(mon, "Kernel not found\n");
    //module_struct = memfrs_get_kernel_info();
    memfrs_gen_pdb_profiles(profile_dir);


}



/******************************************************************
* PURPOSE : Fit the memory at addr into structure fields
******************************************************************/
void do_display_type(Monitor *mon, const QDict *qdict)
{
    const char* struct_name = qdict_get_str(qdict, "struct");
    uint64_t addr = qdict_get_int(qdict, "addr");
    CPUState *thiscpu=NULL;

    thiscpu = ENV_GET_CPU((CPUArchState*)mba_mon_get_cpu());
    memfrs_display_type(thiscpu, addr, struct_name);
}



/******************************************************************
* PURPOSE : List the running process
******************************************************************/
void do_process_list(Monitor *mon, const QDict *qdict)
{
    CPUState *cpu, *thiscpu=NULL;    
    UT_array *proc_list;
    process_list_st *print_proc_list;

    // Find the first CPU
    CPU_FOREACH(cpu)
    {
        monitor_printf(mon, "%p\n", cpu);
        thiscpu = cpu;
        break;
    }

    proc_list = memfrs_enum_proc_list(g_kpcr_ptr, thiscpu);

    if( proc_list != NULL ){
        print_proc_list = NULL;
        monitor_printf(mon, "Eprocess              CR3                 PID   Full Process Path / [Process Name]\n");
        monitor_printf(mon, "--------------------- ------------------- ----- ----------------------------------\n");
        while( (print_proc_list=(process_list_st*)utarray_next(proc_list,print_proc_list)) ){
            monitor_printf(mon, "0x%-20lx%-20lx%-5"PRId64" %s\n", print_proc_list->eprocess, print_proc_list->CR3, print_proc_list->pid, print_proc_list->full_file_path);
        }
        free(proc_list);
    }
    else
        monitor_printf(mon, "Something is wrong, please check error number\n");
}



/******************************************************************
* PURPOSE : Show the process handles
******************************************************************/
void do_process_handles_list(Monitor *mon, const QDict *qdict)
{
    CPUState *cpu, *thiscpu=NULL;
    UT_array *handles;
    handles_st *print_handles;
    handles_node_st *print_handles_node;
    uint64_t handles_cr3 = 0x0;

    const char *target_type = NULL,
          *target = NULL;

    if( qdict_haskey(qdict, "target_type") )
        target_type = qdict_get_str(qdict, "target_type");
    if( qdict_haskey(qdict, "target") )
        target = qdict_get_str(qdict, "target");

    // Find the first CPU
    CPU_FOREACH(cpu)
    {
        monitor_printf(mon, "%p\n", cpu);
        thiscpu = cpu;
        break;
    }


    // Choose parsing type
    if( (qdict_haskey(qdict, "target_type")^qdict_haskey(qdict, "target")) == 1 ){
        monitor_printf(mon, "Wrong format\n");
        return;
    }
    else if(target_type == NULL && target ==NULL)
        handles = memfrs_enum_proc_handles( PARSING_HANDLE_TYPE_ALL, 0, g_kpcr_ptr, thiscpu );
    else{
        if( strcmp(target_type, "-c")==0 || strcmp(target_type, "-C")==0 )
            handles = memfrs_enum_proc_handles( PARSING_HANDLE_TYPE_CR3, (uint64_t)strtoull(target, NULL, 16), g_kpcr_ptr, thiscpu );
        else if( strcmp(target_type, "-e")==0 || strcmp(target_type, "-E")==0 )
            handles = memfrs_enum_proc_handles( PARSING_HANDLE_TYPE_EPROCESS, (uint64_t)strtoull(target, NULL, 16), g_kpcr_ptr, thiscpu );
        else if( strcmp(target_type, "-p")==0 || strcmp(target_type, "-P")==0 )
            handles = memfrs_enum_proc_handles( PARSING_HANDLE_TYPE_PID, (uint64_t)strtoull(target, NULL, 10), g_kpcr_ptr, thiscpu );
        else if( strcmp(target_type, "-t")==0 || strcmp(target_type, "-T")==0 )
            handles = memfrs_enum_proc_handles_detail( PARSING_HANDLE_TYPE_TYPE, target, g_kpcr_ptr, thiscpu );
        else if( strcmp(target_type, "-f")==0 || strcmp(target_type, "-F")==0 )
            handles = memfrs_enum_proc_handles_detail( PARSING_HANDLE_TYPE_FULL_DETAIL, target, g_kpcr_ptr, thiscpu );
        else if( strcmp(target_type, "-d")==0 || strcmp(target_type, "-D")==0 )
            handles = memfrs_enum_proc_handles_detail( PARSING_HANDLE_TYPE_DETAIL, target, g_kpcr_ptr, thiscpu );
        else{
            monitor_printf(mon, "Wrong format\n");
            return;
        }
    }


    if( handles != NULL ){
        print_handles = NULL;
        while( (print_handles=(handles_st*)utarray_next(handles,print_handles)) ){
            if( handles_cr3 == 0x0 || print_handles->CR3 != handles_cr3){
                handles_cr3 = print_handles->CR3;
                monitor_printf(mon, "\nCR3 : %"PRIx64"\tERPOCESS: 0x%"PRIx64 "\tPID: %"PRId64 "\tImage name: %s\n", 
                        print_handles->CR3, print_handles->eprocess, print_handles->pid, print_handles->imagename);
                monitor_printf(mon, "Handle entry address  Index   Granted access Type                      Detail\n");
                monitor_printf(mon, "-------------------- -------- -------------- ------------------------- ----------\n");
            }
            print_handles_node = NULL;
            while( (print_handles_node=(handles_node_st*)utarray_next(print_handles->handles_node, print_handles_node)) ){
                monitor_printf(mon, "0x%-18"PRIx64 " 0x%-6"PRIx16 " 0x%-12"PRIx64 " %-25s %s\n",
                        print_handles_node->handle_table_entry_address, 
                        print_handles_node->handle_table_entry_index, 
                        print_handles_node->grantedaccess, 
                        print_handles_node->type,
                        print_handles_node->detail);
            }
        }
        utarray_free(handles);
    }
    else
        monitor_printf(mon, "Something is wrong, please check error number\n");
}



/******************************************************************
* PURPOSE : List all types of handles
******************************************************************/
void do_handles_types_list(Monitor *mon, const QDict *qdict)
{
    CPUState *cpu, *thiscpu=NULL;
    UT_array *handles_types;
    char **print_handles_types;

    // Find the first CPU
    CPU_FOREACH(cpu)
    {
        monitor_printf(mon, "%p\n", cpu);
        thiscpu = cpu;
        break;
    }

    handles_types = memfrs_enum_handles_types( g_kpcr_ptr, thiscpu );

    if( handles_types !=NULL ){

        monitor_printf(mon, " Handles Types \n");
        monitor_printf(mon, "---------------\n");

        print_handles_types = NULL;
        while ( (print_handles_types=(char**)utarray_next(handles_types,print_handles_types))) {
            if(*print_handles_types!=NULL)
                printf("%s\n", *print_handles_types);
        }
        utarray_free(handles_types); 
    }
    else
        monitor_printf(mon, "Something is wrong, please check error number\n");
}



/******************************************************************
* PURPOSE : List the running network
******************************************************************/
void do_network_list(Monitor *mon, const QDict *qdict)
{
    CPUState *cpu=NULL;
    UT_array *network_list;
    network_state *print_network_list;
    cpu = ENV_GET_CPU((CPUArchState*)mba_mon_get_cpu());

    network_list = memfrs_scan_network(cpu);

    if( network_list != NULL ){
        print_network_list = NULL;
        monitor_printf(mon, "Proto Phymemory       Eprocess         File name     Pid      State                      Local address                      Foreign address          Time\n");
        monitor_printf(mon, "----- ---------- ------------------ --------------- ----- ------------- ------------------------------------------------ ---------------------- ------------------------\n");
        while( (print_network_list=(network_state*)utarray_next(network_list,print_network_list)) ){
            monitor_printf(mon, "%s 0x%-8"PRIx64" 0x%"PRIx64" %-16s %-5"PRIu64" %-12s %-48s %-22s %s",
                    print_network_list->protocol, 
                    print_network_list->pmem, 
                    print_network_list->eprocess, 
                    print_network_list->file_name, 
                    print_network_list->pid, 
                    print_network_list->state, 
                    print_network_list->local_addr, 
                    print_network_list->foreign_addr, 
                    print_network_list->time);
        }
        free(network_list);
    }
    else
        monitor_printf(mon, "Something is wrong, please check error number\n");
}



/******************************************************************
* PURPOSE : List the ssdt
******************************************************************/
void do_ssdt_list(Monitor *mon, const QDict *qdict)
{
    CPUState *cpu, *thiscpu=NULL;
    UT_array *ssdt_list;
    ssdt_list_st *print_ssdt_list;

    // Find the first CPU
    CPU_FOREACH(cpu)
    {
        monitor_printf(mon, "%p\n", cpu);
        thiscpu = cpu;
        break;
    }

    ssdt_list = memfrs_enum_ssdt_list(g_kpcr_ptr, thiscpu);

    if( ssdt_list != NULL ){
        print_ssdt_list = NULL;
        monitor_printf(mon, "Offset  system call addr          system call name\n");
        monitor_printf(mon, "------ ------------------ --------------------------------\n");
        while( (print_ssdt_list=(ssdt_list_st*)utarray_next(ssdt_list,print_ssdt_list)) ){
            monitor_printf(mon, "0x%-4x 0x%-16"PRIx64"  nt!%s\n", 
                    print_ssdt_list->index,
                    print_ssdt_list->address,
                    print_ssdt_list->system_call_name);
        }
        free(ssdt_list);
    }
    else
        monitor_printf(mon, "Something is wrong, please check error number\n");
}
