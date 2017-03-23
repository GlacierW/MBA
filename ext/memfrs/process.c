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
#include "process.h"



static void process_list_dtor(void *_elt) {
    process_list_st *elt = (process_list_st*)_elt;
    if(elt->full_file_path) free(elt->full_file_path);
}
UT_icd proc_list_icd = {sizeof(process_list_st), NULL, NULL, process_list_dtor};
/*****************************************************************n
UT_array* memfrs_enum_proc_list( uint64_t kpcr_ptr, CPUState *cpu )

Eumerate the running process

INPUT:     uint64_t kpcr_ptr,        the address of _KPCR struct
           CPUState *cpu,            the pointer to current cpu
OUTPUT:    int,                      return 0 if sucess, and not 0 otherwise
*******************************************************************/
UT_array* memfrs_enum_proc_list( uint64_t kpcr_ptr, CPUState *cpu )
{
    UT_array *list = NULL;
    
    process_list_st proc_list;

    int i;
    uint64_t kthread_ptr,
             eprocess_ptr,
             eprocess_ptr_init = 0,
             buf_ptr
                 ;

    uint64_t cr3,
             processid;

    // max length of file name is 15
    uint8_t file_name_buf[32];
    uint8_t file_path_buf[256];
    uint16_t length;

    int offset_entry_list_to_eprocess = 0;
    int count = 0;

    //Check if the data structure information is loaded
    if(g_struct_info ==NULL)
    {
        memfrs_errno = MEMFRS_ERR_NOT_LOADED_GLOBAL_STRUCTURE;
        return NULL;
    }

    //Check if kpcr is already found
    if(kpcr_ptr == 0)
    {
        memfrs_errno = MEMFRS_ERR_NOT_FOUND_KPCR;
        return NULL;
    }

    //Check the cpu pointer valid
    if(cpu == NULL)
    {
        memfrs_errno = MEMFRS_ERR_INVALID_CPU;
        return NULL;
    }

    // Read the concrete memory value of kthread_ptr(CurrentThread) via _KPCR address
    memfrs_get_mem_struct_content( cpu, 0, (uint8_t*)&kthread_ptr, sizeof(kthread_ptr), kpcr_ptr, false, "_KPCR", 2, "#Prcb", "#CurrentThread");

    // Read the concrete memory value of PROCESS via CurrentThread
    // Get the first PROCESS
    memfrs_get_mem_struct_content( cpu, 0, (uint8_t*)&eprocess_ptr, sizeof(eprocess_ptr), kthread_ptr, false, "_KTHREAD", 1, "#Process");

    // Assign process_list be a 'process_list_st' structure UTarray
    utarray_new( list, &proc_list_icd);


    // Start iteration process list
    eprocess_ptr_init = eprocess_ptr;

    do {

        //Read CR3 & Process name
        memfrs_get_mem_struct_content( cpu, 0, (uint8_t*)&cr3, sizeof(cr3), eprocess_ptr, false, "_EPROCESS", 2, "#Pcb", "#DirectoryTableBase");
        memfrs_get_mem_struct_content( cpu, 0, (uint8_t*)&processid, sizeof(processid), eprocess_ptr, false, "_EPROCESS", 1, "#UniqueProcessId");
        memfrs_get_mem_struct_content( cpu, 0, (uint8_t*)file_name_buf, sizeof(file_name_buf), eprocess_ptr, false, "_EPROCESS", 1, "#ImageFileName");
        //printf( "0x%-20lx%-20lx%-5"PRId64" ", eprocess_ptr, cr3, processid );

        if( cr3 !=0 ){
            if ( memfrs_get_mem_struct_content(cpu, cr3, (uint8_t*)&length, sizeof(length), eprocess_ptr, false, "_EPROCESS", 4, "*Peb", "*ProcessParameters", "#ImagePathName", "#Length") == -1 )
                length =0;
            if ( memfrs_get_mem_struct_content(cpu, cr3, (uint8_t*)&buf_ptr, sizeof(buf_ptr), eprocess_ptr, false, "_EPROCESS", 4, "*Peb", "*ProcessParameters", "#ImagePathName", "*Buffer") == -1 )
                buf_ptr =0;
        }

        if( memfrs_get_virmem_content( cpu, cr3, buf_ptr, sizeof(file_path_buf), (uint8_t*)file_path_buf ) != -1 ){
            proc_list.full_file_path=(char*)malloc(length/2+1);
            for(i=0;i<length;i=i+1){
                if( i%2==0 )
                    proc_list.full_file_path[i/2]=(char)(*(file_path_buf+i));
            }
            proc_list.full_file_path[length/2]='\0';
        }

        // [TODO] Image file path sometimes will stored in unvalid address for unknow reason
        else {
            proc_list.full_file_path = (char*)malloc(32);
            sprintf(proc_list.full_file_path, "[Process Name] %-15s", file_name_buf);
        }

        proc_list.eprocess = eprocess_ptr;
        proc_list.CR3 = cr3;
        proc_list.pid = processid;
        utarray_push_back(list, &proc_list);

        count++;
        if(count > 1000)
            break;

        // Read next entry
        memfrs_get_mem_struct_content( cpu, 0, (uint8_t*)&eprocess_ptr, sizeof(eprocess_ptr), eprocess_ptr, false, "_EPROCESS", 2, "#ActiveProcessLinks", "*Blink");
        // Substract entry_list offset to find base address of eprocess
        memfrs_get_nested_field_offset(&offset_entry_list_to_eprocess, "_EPROCESS", 1, "ActiveProcessLinks");
        eprocess_ptr = eprocess_ptr-offset_entry_list_to_eprocess;

    }while( eprocess_ptr != eprocess_ptr_init );

    return list;
}

