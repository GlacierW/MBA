/*
 *  MBA Virtual Machine Memory Introspection implementation
 *
 *  Copyright (c)   2016 ELin Ho
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
#include "exec/cpu-all.h"

#include "memfrs.h"
#include "memfrs-priv.h"
#include "ssdt.h"


static void ssdt_list_dtor(void *_elt) {
    ssdt_list_st *elt = (ssdt_list_st*)_elt;
    if(elt->system_call_name) free(elt->system_call_name);
}
UT_icd ssdt_list_icd = {sizeof(ssdt_list_st), NULL, NULL, ssdt_list_dtor};
/*****************************************************************n
UT_array* memfrs_enum_ssdt_list( uint64_t kpcr_ptr, CPUState *cpu )

Eumerate the ssdt

INPUT:     uint64_t kpcr_ptr,        the address of _KPCR struct
           CPUState *cpu,            the pointer to current cpu
OUTPUT:    UT_array*                 return a UT_array with ssdt data
*******************************************************************/
UT_array* memfrs_enum_ssdt_list( uint64_t kpcr_ptr, CPUState *cpu )
{
    UT_array *list = NULL;
    ssdt_list_st ssdt_list;

    int i, j;

    const char* syscall_table_name = "KiServiceTable";
    const char* syscall_count_name = "KiServiceLimit";
    uint64_t base = 0;
    uint64_t addr_KiServiceTable = 0;
    uint64_t addr_KiServiceLimit = 0;
    json_object *gvar = NULL;

    uint64_t syscall_addr;
    uint16_t syscall_count;
    int32_t  syscall_tmp_addr;
    char    *syscall_name;
    size_t   syscall_len;

    // Check if the data structure information is loaded
    if(g_struct_info ==NULL)
    {
        memfrs_errno = MEMFRS_ERR_NOT_LOADED_GLOBAL_STRUCTURE;
        return NULL;
    }

    // Check if kpcr is already found
    if(kpcr_ptr == 0)
    {
        memfrs_errno = MEMFRS_ERR_NOT_FOUND_KPCR;
        return NULL;
    }

    // Check the cpu pointer valid
    if(cpu == NULL)
    {
        memfrs_errno = MEMFRS_ERR_INVALID_CPU;
        return NULL;
    }


    // Kernel base
    if( (base = memfrs_get_nt_kernel_base()) == 0 && 
        (base = memfrs_find_nt_kernel_base(cpu)) ==0 ){
        memfrs_errno = MEMFRS_ERR_NOT_FOUND_KERNEL_BASE;
        return NULL;
    }



    // Get syetem call table address
    gvar = memfrs_q_globalvar(syscall_table_name);
    if(gvar ==NULL){
        memfrs_errno = MEMFRS_ERR_NOT_FOUND_GLOBAL_STRUCTURE;
        return NULL;
    }
    else{
        addr_KiServiceTable = memfrs_gvar_offset(gvar) + base;
    }

    // Get syetem call count address
    gvar = memfrs_q_globalvar(syscall_count_name);
    if(gvar ==NULL){
        memfrs_errno = MEMFRS_ERR_NOT_FOUND_GLOBAL_STRUCTURE;
        return NULL;
    }
    else{
        addr_KiServiceLimit = memfrs_gvar_offset(gvar) + base;
    }

    // Get system call count
    if( cpu_memory_rw_debug(cpu, addr_KiServiceLimit, (uint8_t*)&syscall_count, sizeof(syscall_count), 0) != 0 ){
        memfrs_errno = MEMFRS_ERR_MEMORY_READ_FAILED;
        return NULL;
    }


    utarray_new( list, &ssdt_list_icd);

    for( i=0 ; i<syscall_count ; i=i+1 ){
        //if( cpu_memory_rw_debug((CPUState *)&copied_cpu, addr_KiServiceTable + i*0x4 , (uint8_t*)&syscall_tmp_addr, sizeof(syscall_tmp_addr), 0) == 0 ){
        if( cpu_memory_rw_debug(cpu, addr_KiServiceTable + i*0x4 , (uint8_t*)&syscall_tmp_addr, sizeof(syscall_tmp_addr), 0) == 0 ){
            syscall_addr = (syscall_tmp_addr>>4) + addr_KiServiceTable;

            // Get system call name
            reverse_symbol* sym_rev_hash = NULL;
            sym_rev_hash = memfrs_build_gvar_lookup_map();
            syscall_name = memfrs_get_symbolname_via_address( sym_rev_hash, syscall_addr - base);
            memfrs_free_reverse_lookup_map(sym_rev_hash);
            if( !syscall_name ){
                printf("[%d]\n", i);
            }


            // Insert datas to ssdt structure
            ssdt_list.index = i;
            ssdt_list.address = syscall_addr;
            syscall_len = strlen(syscall_name);
            ssdt_list.system_call_name = (char*)malloc(syscall_len+1);
            for(j=0;j<syscall_len;j=j+1)
                ssdt_list.system_call_name[j] = syscall_name[j];
            ssdt_list.system_call_name[syscall_len] = '\0';

            // Add datas to utarray
            utarray_push_back(list, &ssdt_list);
        }
    }

    return list;
}
