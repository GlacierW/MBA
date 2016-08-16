/*
 *  MBA Virtual Machine Memory Introspection implementation
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

#include "qemu-common.h"
#include "monitor/monitor.h"
#include "include/exec/cpu-common.h"
#include "include/utarray.h"
#include "exec/cpu-all.h"

#include "json.h"

#include "ext/memfrs/memfrs.h"

/* Global Variable */
uint64_t g_kpcr_ptr = 0;
json_object *g_struct_info = NULL;


bool memfrs_check_struct_info(void)
{
    return (g_struct_info!=NULL)? 1 : 0;
}

/*******************************************************************
field_info* memfrs_q_field( json_object* struc, const char* field_name  )

Given the structure's json object, q_field return the field information
of given field_name.

INPUT: json_object* struc       json object of structure we want to query
       const char* field_name   the target name we want to find

OUTPUT: field_info*             return the field information in type if field_info
*******************************************************************/
field_info* memfrs_q_field( json_object* struc, const char* field_name )
{
    json_object* target = NULL;
    json_object* tmp_jobject = NULL;
    field_info* f_info = NULL;
    int offset = 0;

    // Query the field in the structures(struc) by the field_name
    // Save the result into target json object
    json_object_object_get_ex(struc, field_name, &target);

    // The result is the json array as ( field_type, field_offset, field_size, field_is_pointer)
    // query and unpack offset
    tmp_jobject = json_object_array_get_idx(target, 1);
    offset = json_object_get_int(tmp_jobject);

    f_info =(field_info*)malloc(sizeof(field_info));
    f_info->offset = offset;

    //query and unpack field type
    tmp_jobject = json_object_array_get_idx(target, 0);
    strncpy(f_info->type_name, json_object_get_string(tmp_jobject), STRLEN); 

    //TODO: type size leave empty now 


    // Put the json object of field type into fielf_info structure
    if(g_struct_info != NULL)
        json_object_object_get_ex(g_struct_info, f_info->type_name, &(f_info->jobject_type));

    return f_info;
}

/*******************************************************************
int memfrs_close_field(field_info* field)

free the memory of field_info.

INPUT:     field_info* field,   pointer of field_info object to be freed 
OUTPUT:    int,                 return 0 if sucess, and not 0 otherwise       

*******************************************************************/
int memfrs_close_field(field_info* field)
{
    free(field);
    return 0;
}

/*******************************************************************
json_object* memfrs_q_struct(const char* ds_name)

query the data structure's info via given structure name

INPUT:    const char* ds_name,  the name of interesting structure
OUTPUT:   json_object*,         json object representation of the target struct

*******************************************************************/
json_object* memfrs_q_struct(const char* ds_name)
{
    json_object* target = NULL;
    
    // Query global structure info with structure name ds_name
    // Restore the query result into target json_object 
    json_object_object_get_ex(g_struct_info, ds_name, &target);
    return target;
}

/*******************************************************************
int memfrs_load_structs( const char* type_filename)

Load the data structure information into g_struct_info.

INPUT:     const char* type_filename,  the filename of json data structure database
OUTPUT:    int,                        return 0 if sucess, and not 0 otherwise

*******************************************************************/
int memfrs_load_structs( const char* type_filename)
{
    g_struct_info = json_object_from_file(type_filename);
    return 0;
}

/*******************************************************************
bool memfrs_kpcr_self_check( uint64_t kpcr_ptr ) 

Hueristic check if certain address contain the data structure _KPCR

INPUT:     uint64_t kpcr_ptr,        the 64bit address of possible KPCR pointer
OUTPUT:    bool,                     return true if kpcr found, else retuen false 
*******************************************************************/
bool memfrs_kpcr_self_check( uint64_t kpcr_ptr ) {

    uint64_t self_ptr = 0;
    json_object* jkpcr =NULL;
    field_info* f_info = NULL;
    int offset_self_to_kpcr = 0;

    //Check if the global data structure info is load, if not abort check.
    if(g_struct_info == NULL)
    {
        return false;
    }

    // Find the struct _KPCR
    jkpcr = memfrs_q_struct("_KPCR");

    // Query field name Self in _KPCR structure
    f_info = memfrs_q_field(jkpcr, "Self");
    offset_self_to_kpcr = f_info->offset;
    memfrs_close_field(f_info);

    // Read the concrete memory value in Self field
    if( cpu_memory_rw_debug(current_cpu, kpcr_ptr + offset_self_to_kpcr, (uint8_t*)&self_ptr, sizeof(self_ptr), 0) != 0 )
    {
        g_kpcr_ptr = 0;
        return false;
    }  

    // Check if the Self pointer point back to _KPCR structure, which is the hueristic check of _KPCR 
    if( kpcr_ptr == self_ptr ) {
        g_kpcr_ptr = kpcr_ptr;
        printf("KPCR found %" PRIx64 "\n", g_kpcr_ptr);
        return true;
    }
  
    g_kpcr_ptr = 0;
    return false;
}

//TODO: Still buggy
UT_icd adr_icd = {sizeof(uint64_t), NULL, NULL, NULL };
UT_array* memfrs_scan_virmem( CPUState *cpu, uint64_t start_addr, uint64_t end_addr, const char* pattern ) {
    uint64_t i;
    uint8_t* buf = (uint8_t*)malloc(strlen(pattern));
    UT_array *match_addr;

    memset(buf, 0, strlen(pattern));

    if(buf == NULL){
        printf("Cannot allocate memory for do_show_memory_taint_map()\n");
        return NULL;
    }

    utarray_new( match_addr, &adr_icd);

    printf("Scan for pattern %s\n", pattern);

    for(i = start_addr; i < end_addr-strlen(pattern)+1; i++)
    {
        //if(i %0x1000 == 0)
        //    printf("Current addr %"PRIx64"\n", i);
        cpu_memory_rw_debug(cpu, i, buf, strlen(pattern), 0);
        if(memcmp(buf, pattern, strlen(pattern))==0)
        {
            printf("pattern found %"PRIx64"\n", i);
            utarray_push_back(match_addr, &i);
        }
    }
    return match_addr;
}

/*******************************************************************
UT_array* memfrs_scan_phymem( uint64_t start_addr, uint64_t end_addr, const char* pattern )

Scan for specific pattern in the VM's physical memory

INPUT:    uint64_t start_addr,  The start address
          uint64_t end_addr,    the end address
          const char* pattern   pattern to search, support only ascii string
OUTPUT:   UT_array*,            An UT_array that contains the address of found pattern

*******************************************************************/
UT_array* memfrs_scan_phymem( uint64_t start_addr, uint64_t end_addr, const char* pattern ) {
    uint64_t i;
    UT_array *match_addr;
    uint8_t* buf = (uint8_t*)malloc(strlen(pattern));
    if(buf == NULL){
        printf("Cannot allocate memory for memfrs_scan_phymem()\n");
        return NULL;
    }

    utarray_new( match_addr, &adr_icd);

    printf("Scan for pattern %s\n", pattern);
    for(i = start_addr; i < end_addr-strlen(pattern)+1; i++)
    {
        cpu_physical_memory_read(i, buf, strlen(pattern));
        if(memcmp(buf, pattern, strlen(pattern))==0)
        {
            printf("pattern found %"PRIx64"\n", i);
            utarray_push_back(match_addr, &i);
        }
    }
    return match_addr;
}

/*******************************************************************
void memfrs_get_virmem_content( CPUState *cpu, uint64_t cr3, uint64_t target_addr, uint64_t target_length, uint8_t* buf)

Get the memory content in virtual memory

INPUT:    CPUState *cpu          Current cpu
          uint64_t cr3           CR3 value, 0 if no specific process
          uint64_t target_addr   The target address 
          uint64_t target_length The length to be getten
          uint8_t* buf           The buffer to save the memory content
OUTPUT:   void

*******************************************************************/
void memfrs_get_virmem_content( CPUState *cpu, uint64_t cr3, uint64_t target_addr, uint64_t target_length, uint8_t* buf)
{
    X86CPU copied_cpu;
    int ret;
    memcpy(&copied_cpu, X86_CPU(cpu), sizeof(copied_cpu));

    if(cr3 != 0)
    {
        copied_cpu.env.cr[3] = cr3;
    }

    ret = cpu_memory_rw_debug((CPUState *)&copied_cpu, target_addr, (uint8_t*)buf, target_length, 0);
    if(ret != 0)
        printf("Fail to read virtual memory\n");

    return;
}

void hexdump(Monitor *mon, uint8_t* buf, size_t length)
{
    int i,j ;

    for(i = 0 ; i < length ; i+=0x10) {
        monitor_printf(mon, "%02x: ", i);
        for(j = 0; j< 0x10; j++){
            if(i+j > length)
                monitor_printf( mon, "   " );
            else
                monitor_printf( mon, "%02x " , buf[i+j]);
        }

        monitor_printf(mon, "  |  ");

        for(j = 0; j< 0x10; j++){
            if(i+j > length)
                monitor_printf( mon, "-" );
            else if(buf[i+j] >= 0x20 && buf[i+j] <= 0x7e)
                monitor_printf( mon, "%c" , buf[i+j]);
            else
                monitor_printf( mon, "." );
        }

        monitor_printf(mon, "|\n");
    }

}

/*******************************************************************
int memfrs_enum_proc_list( uint64_t kpcr_ptr, CPUState *cpu ) 

Eumerate the running process
 
INPUT:     uint64_t kpcr_ptr,        the address of _KPCR struct 
           CPUState *cpu,            the pointer to current cpu
OUTPUT:    int,                      return 0 if sucess, and not 0 otherwise
*******************************************************************/
int memfrs_enum_proc_list( uint64_t kpcr_ptr, CPUState *cpu ) 
{
    uint64_t kthread_ptr,
             eprocess_ptr;

    uint64_t cr3;

    uint8_t buf[STRLEN];

    uint64_t eprocess_ptr_init = 0;

    json_object* jkpcr =NULL;
    int offset_curthread_to_kpcr = 0;

    json_object* jkthread =NULL;
    int offset_process_to_kthread = 0;

    json_object* jeprocess = NULL;
    int offset_cr3_to_eprocess = 0;
    int offset_blink_to_eprocess = 0;
    int offset_entry_list_to_eprocess = 0;
    int offset_process_name_to_eprocess = 0;
    field_info* f_info, *f_info2 = NULL;

    //Check if the data structure information is loaded
    if(g_struct_info ==NULL)
    {
        printf("Data structure information is not loaded\n");
        return -1;
    }

    //Check if kpcr is already found
    if(kpcr_ptr == 0)
    {
        printf("Not yet find kpcr address\n");
        return -1;
    }

    //Check the cpu pointer valid
    if(cpu == NULL)
    {
        printf("CPU state is null\n");
        return -1;
    }

    // Retrieve the _KPCR structure
    jkpcr = memfrs_q_struct("_KPCR");
    // Query Prcb field in _KPCR struct
    f_info = memfrs_q_field(jkpcr, "Prcb");
    // Query Prcb for the sub-field name CurrentThread 
    f_info2 = memfrs_q_field(f_info->jobject_type, "CurrentThread");
    // Calculating the offset of CurrentThread in _KPCR 
    offset_curthread_to_kpcr = f_info->offset + f_info2->offset;
    //Cleaning ...
    memfrs_close_field(f_info2);
    memfrs_close_field(f_info);

    // Retrieve the _KTHREAD structure
    jkthread = memfrs_q_struct("_KTHREAD");
    // Query PROCESS field in  _KTHREAD struct
    f_info = memfrs_q_field(jkthread, "PROCESS");
    offset_process_to_kthread = f_info->offset;
    memfrs_close_field(f_info);
    
    // Read the concrete memory value of kthread_ptr(CurrentThread) via _KPCR address
    cpu_memory_rw_debug( cpu, kpcr_ptr + offset_curthread_to_kpcr, (uint8_t*)&kthread_ptr, sizeof(kthread_ptr), 0 );
    // Read the concrete memory value of PROCESS via CurrentThread 
    // Get the first PROCESS 
    cpu_memory_rw_debug( cpu, kthread_ptr + offset_process_to_kthread, (uint8_t*)&eprocess_ptr, sizeof(eprocess_ptr), 0 );

    // Retrieve the _EPROCESS structure 
    jeprocess = memfrs_q_struct("_EPROCESS");
    // Query Pcb field in _EPROCESS struct
    f_info = memfrs_q_field(jeprocess, "Pcb");
    // Query Pcb for the sub-field name DirectoryTableBase, which is CR3 value
    f_info2 = memfrs_q_field(f_info->jobject_type, "DirectoryTableBase");
    offset_cr3_to_eprocess = f_info->offset + f_info2->offset;
    memfrs_close_field(f_info2);
    memfrs_close_field(f_info);

    // Query ActiveProcessLinks in _EPROCESS
    f_info = memfrs_q_field(jeprocess, "ActiveProcessLinks");
    // Query Blink in ActiveProcessLinks
    f_info2 = memfrs_q_field(f_info->jobject_type, "Blink");
    // Calculate Entry List offset from eprocess
    offset_entry_list_to_eprocess = f_info->offset;
    // Calculate Blink offset from eprocess
    offset_blink_to_eprocess = f_info->offset + f_info2->offset;
    memfrs_close_field(f_info2);
    memfrs_close_field(f_info);

    // Query and Cal offset of ImageFileName in _EPROCESS struct
    f_info = memfrs_q_field(jeprocess, "ImageFileName");
    offset_process_name_to_eprocess = f_info->offset;
    memfrs_close_field(f_info);

    // Start iteration process list
    eprocess_ptr_init = eprocess_ptr;

    do {
        //Read CR3 & Process name
        cpu_memory_rw_debug( cpu, eprocess_ptr + offset_cr3_to_eprocess, (uint8_t*)&cr3, sizeof(cr3), 0 );
        cpu_memory_rw_debug( cpu, eprocess_ptr + offset_process_name_to_eprocess, (uint8_t*)buf, sizeof(buf), 0 );
        printf( "eprocess: %" PRIx64 "CR3: %" PRIx64 ", Process Name: %s\n", eprocess_ptr, cr3, buf );

        // read next entry  
        cpu_memory_rw_debug( cpu, eprocess_ptr + offset_blink_to_eprocess, (uint8_t*)&eprocess_ptr, sizeof(eprocess_ptr), 0 );
        // Substract entry_list offset to find base address of eprocess
        eprocess_ptr -= offset_entry_list_to_eprocess;

    } while( eprocess_ptr != eprocess_ptr_init );

    return 0;
}

void parse_unicode_strptr(uint64_t ustr_ptr, CPUState *cpu)
{
    json_object* ustr = NULL;
    field_info* f_info = NULL;
    uint16_t length, max_length;
    uint64_t buf_ptr;
    int offset;
    uint8_t *buf;
    char* str;
    int i;

    ustr = memfrs_q_struct("_UNICODE_STRING");

    f_info = memfrs_q_field(ustr, "MaximumLength");
    offset = f_info->offset;
    cpu_memory_rw_debug( cpu, ustr_ptr+offset, (uint8_t*)&max_length, sizeof(max_length), 0 );
    memfrs_close_field(f_info);

    f_info = memfrs_q_field(ustr, "Length");
    offset = f_info->offset;
    cpu_memory_rw_debug( cpu, ustr_ptr+offset, (uint8_t*)&length, sizeof(length), 0 );
    memfrs_close_field(f_info);
  
    printf("String with size %d/%d\n", length, max_length);

    if(length == 0 || length > 256 || max_length ==0 || max_length > 256)
        return;

    f_info = memfrs_q_field(ustr, "Buffer");
    offset = f_info->offset;
    cpu_memory_rw_debug( cpu, ustr_ptr+offset, (uint8_t*)&buf_ptr, sizeof(buf_ptr), 0 );
    memfrs_close_field(f_info); 

    //printf("Address: %" PRIx64 "\n", buf_ptr);

    buf = (uint8_t*)malloc(max_length+2);
    str = (char*)malloc(max_length+1);
    memset(str, 0, max_length+1);
    cpu_memory_rw_debug( cpu, buf_ptr, buf, max_length, 0 );
    //Hardcode Unicode Parse
    //wcstombs(str, (const wchar_t *)buf, max_length);
    for(i=0; i<max_length;i+=2)
        str[i/2] = buf[i];   
    str[i] = 0x00;
    //printf("Filename %ls\n", (wchar_t*p)buf);
    printf("Filename %s\n", str);
}

void parse_unicode_str(uint8_t* ustr, CPUState *cpu)
{
    json_object* justr = NULL;
    field_info* f_info = NULL;
    uint16_t length, max_length;
    uint64_t buf_ptr;
    int offset;
    uint8_t *buf;
    char* str;
    int i;

    //printf("String va:  %" PRIx64 "\n", ustr_ptr);

    justr = memfrs_q_struct("_UNICODE_STRING");

    f_info = memfrs_q_field(justr, "MaximumLength");
    offset = f_info->offset;
    max_length = *((uint16_t*)(ustr+offset));
    //cpu_memory_rw_debug( cpu, ustr_ptr+offset, (uint8_t*)&max_length, sizeof(max_length), 0 );
    memfrs_close_field(f_info);

    f_info = memfrs_q_field(justr, "Length");
    offset = f_info->offset;
    length = *((uint16_t*)(ustr+offset));
    //cpu_memory_rw_debug( cpu, ustr_ptr+offset, (uint8_t*)&length, sizeof(length), 0 );
    memfrs_close_field(f_info);

    printf("String with size %d/%d\n", length, max_length);

    if(length == 0 || length > 256 || max_length ==0 || max_length > 256)
        return;

    f_info = memfrs_q_field(justr, "Buffer");
    offset = f_info->offset;
    buf_ptr = *((uint64_t*)(ustr+offset));
    //cpu_memory_rw_debug( cpu, ustr_ptr+offset, (uint8_t*)&buf_ptr, sizeof(buf_ptr), 0 );
    memfrs_close_field(f_info);

    //printf("Address: %" PRIx64 "\n", buf_ptr);

    buf = (uint8_t*)malloc(max_length+2);
    str = (char*)malloc(max_length+1);
    memset(str, 0, max_length+1);
    cpu_memory_rw_debug( cpu, buf_ptr, buf, max_length, 0 );
    //Hardcode Unicode Parse
    //wcstombs(str, (const wchar_t *)buf, max_length);
    for(i=0; i<max_length;i+=2)
        str[i/2] = buf[i];
    str[i] = 0x00;
    //printf("Filename %ls\n", (wchar_t*p)buf);
    printf("Filename %s\n", str);
}

