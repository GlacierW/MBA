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


#if !defined(CONFIG_MEMFRS_TEST)
#include "qemu-common.h"
#include "monitor/monitor.h"
#include "include/exec/cpu-common.h"
#include "exec/cpu-all.h"
#include "include/utarray.h"
#include "include/uthash.h"
#include "json.h"
#endif

#if defined(CONFIG_MEMFRS_TEST)
#include "test/test.h"
#endif

#include "memfrs.h"
#include "memfrs-priv.h"

#if defined(CONFIG_MEMFRS_TEST)
/// Change name to avoid macros in test.h from expanding.
/// Refer to _dift_log to call original dift_log in tests.
#define _MOCKABLE(x) _##x
#else
#define _MOCKABLE(x) x
#endif

#include "ext/dift/dift-commands.h"

#include<stdlib.h>

/* Global Variable */
uint64_t g_kpcr_ptr = 0;
json_object *g_struct_info = NULL;
json_object *g_globalvar_info = NULL;


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
    if(target == NULL)
    {
        printf("\"%s\" not found\n", field_name);
        return NULL;
    }

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
    
    if(target==NULL)
        printf("%s not found\n", ds_name);
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
        printf("KPCR found %lx\n", g_kpcr_ptr);
        return true;
    }

    g_kpcr_ptr = 0;
    return false;
}

//TODO: Still buggy
UT_icd adr_icd = {sizeof(uint64_t), NULL, NULL, NULL };
UT_array* memfrs_scan_virmem( CPUState *cpu, uint64_t start_addr, uint64_t end_addr, const char* pattern, int length ) {
    uint64_t i;

    if(start_addr >= end_addr){
        printf("end_addr is not less than start_addr\n");
        return NULL;
    }

    uint8_t* buf = (uint8_t*)malloc(length);
    UT_array *match_addr;

    memset(buf, 0, length);

    if(buf == NULL){
        printf("Cannot allocate memory for do_show_memory_taint_map()\n");
        return NULL;
    }

    utarray_new( match_addr, &adr_icd);

    printf("Scan for pattern %s\n", pattern);

    for(i = start_addr; i < end_addr-length+1; i++)
    {
        cpu_memory_rw_debug(cpu, i, buf, length, 0);
        if(memcmp(buf, pattern, length)==0)
        {
            printf("pattern found %lx\n", i);
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
UT_array* memfrs_scan_phymem( uint64_t start_addr, uint64_t end_addr, const char* pattern , int length ) {
    uint64_t i;
    UT_array *match_addr;
    if(start_addr >= end_addr){
        printf("end_addr is not less than start_addr\n");
        return NULL;
    }

    uint8_t* buf = (uint8_t*)malloc(length);
    if(buf == NULL){
        printf("Cannot allocate memory for memfrs_scan_phymem()\n");
        return NULL;
    }

    utarray_new( match_addr, &adr_icd);

    printf("Scan for pattern %s\n", pattern);
    for(i = start_addr; i < end_addr-length+1; i++)
    {
        cpu_physical_memory_read(i, buf, length);
        if(memcmp(buf, pattern, length)==0)
        {
            printf("pattern found %lx\n", i);
            utarray_push_back(match_addr, &i);
        }
    }
    return match_addr;
}

/*******************************************************************
int memfrs_get_virmem_content( CPUState *cpu, uint64_t cr3, uint64_t target_addr, uint64_t target_length, uint8_t* buf)

Get the memory content in virtual memory

INPUT:    CPUState *cpu          Current cpu
          uint64_t cr3           CR3 value, 0 if no specific process
          uint64_t target_addr   The target address 
          uint64_t target_length The length to be getten
          uint8_t* buf           The buffer to save the memory content
OUTPUT:   int                    -1 indicate fails

*******************************************************************/
int memfrs_get_virmem_content( CPUState *cpu, uint64_t cr3, uint64_t target_addr, uint64_t target_length, uint8_t* buf)
{
    X86CPU copied_cpu;
    int ret;
    memcpy(&copied_cpu, X86_CPU(cpu), sizeof(copied_cpu));

    if(cr3 != 0)
    {
        copied_cpu.env.cr[3] = cr3;
    }

    ret = cpu_memory_rw_debug((CPUState *)&copied_cpu, target_addr, (uint8_t*)buf, target_length, 0);
    if(ret != 0){
        //printf("Fail to read virtual memory\n");
        return -1;
    }
    return 0;
}

void hexdump(Monitor *mon, uint8_t* buf, size_t length)
{
    int i,j ;

    for(i = 0 ; i < (int)length ; i+=0x10) {
        monitor_printf(mon, "%02x: ", i);
        for(j = 0; j< 0x10; j++){
            if(i+j > (int)length)
                monitor_printf( mon, "   " );
            else
                monitor_printf( mon, "%02x " , buf[i+j]);
        }

        monitor_printf(mon, "  |  ");

        for(j = 0; j< 0x10; j++){
            if(i+j > (int)length)
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
    int i, j, k;
    uint64_t kthread_ptr,
             eprocess_ptr,
             peb_ptr,
             rtl_ptr,
             buf_ptr,
             handle_table_ptr,
             tablecode_ptr
            ;

    CPUX86State* x86_cpu = (CPUX86State*)cpu->env_ptr;
    uint64_t cr3;
    uint64_t cr3_init;

    uint8_t buf[256];
    uint8_t length;

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
    int count = 0;
    field_info* f_info, *f_info2, *f_info3, *f_info4 = NULL;

    int offset_peb_to_eprocess = 0;
    int offset_parameter_to_peb = 0;
    int offset_length_to_rtl = 0;
    int offset_buffer_to_rtl = 0;

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
    f_info = memfrs_q_field(jkthread, "Process");
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



    f_info = memfrs_q_field(jeprocess, "Peb");
    offset_peb_to_eprocess = f_info->offset;
    f_info2 = memfrs_q_field(f_info->jobject_type, "ProcessParameters");
    offset_parameter_to_peb = f_info2->offset;
    f_info3 = memfrs_q_field(f_info2->jobject_type, "ImagePathName");
    f_info4 = memfrs_q_field(f_info3->jobject_type, "Length");
    offset_length_to_rtl = f_info3->offset + f_info4->offset;
    f_info4 = memfrs_q_field(f_info3->jobject_type, "Buffer");
    offset_buffer_to_rtl = f_info3->offset + f_info4->offset;

    memfrs_close_field(f_info4);
    memfrs_close_field(f_info3);
    memfrs_close_field(f_info2);
    memfrs_close_field(f_info);

    // Start iteration process list
    eprocess_ptr_init = eprocess_ptr;

    cr3_init = x86_cpu->cr[3];

    do {
        //Read CR3 & Process name
        cpu_memory_rw_debug( cpu, eprocess_ptr + offset_cr3_to_eprocess, (uint8_t*)&cr3, sizeof(cr3), 0 );
        cpu_memory_rw_debug( cpu, eprocess_ptr + offset_process_name_to_eprocess, (uint8_t*)buf, sizeof(buf), 0 );
        printf( "eprocess: %lx CR3: %lx, Process Name: %s\n", eprocess_ptr, cr3, buf );

        if( cr3 !=0 ){

//--
uint64_t kcb_ptr, parent_kcb_ptr, check_ptr;
uint64_t nameblock_ptr;
uint16_t key_name_length;
uint8_t key_name_buf[256];
//--
unsigned int uniqueProcess, uniqueThread;
//--
uint64_t nameinfo_ptr;
uint8_t nameinfo_length, nameinfo_buf[256];
//--
uint64_t level0_ptr, level1_ptr, DeviceObject_ptr, device_infomask;
uint64_t file_ptr, device_file_ptr;
int pointercount, handlecount;
uint8_t typeindex;
uint8_t infomask;
uint64_t grantedaccess;
uint8_t filelength, device_filelength;
uint8_t file_buf[256], device_file_buf[256];
int table_level, true_type;

        cpu_memory_rw_debug( cpu, eprocess_ptr + 0x418, (uint8_t*)&handle_table_ptr, sizeof(handle_table_ptr), 0 );
        cpu_memory_rw_debug( cpu, handle_table_ptr + 0x8, (uint8_t*)&tablecode_ptr, sizeof(tablecode_ptr), 0 );
        //printf("Handle_Table: %" PRIx64 "\n", handle_table_ptr);
        //printf("Tablecode: %" PRIx64 "\n", tablecode_ptr);

//cpu_memory_rw_debug( cpu, 0xfffff80399254440, (uint8_t*)&typeindex, 1, 0 );

x86_cpu->cr[3] = cr3;
        table_level = tablecode_ptr & 0x3;
        tablecode_ptr = tablecode_ptr & 0xfffffffffffffffc ;
        if( table_level == 0 ){
            for( i=0 ; i<256 ; i=i+1 ){
                tablecode_ptr = tablecode_ptr+0x10;
                cpu_memory_rw_debug( cpu, tablecode_ptr, (uint8_t*)&level0_ptr, sizeof(level0_ptr), 0 );
                level0_ptr = ( ( (level0_ptr & 0xfffffffffff00000) >> 16 ) + 0xffff000000000000);
                cpu_memory_rw_debug( cpu, tablecode_ptr+0x8, (uint8_t*)&grantedaccess, sizeof(grantedaccess), 0 );
                    grantedaccess = grantedaccess & 0x0000000001ffffff;

                cpu_memory_rw_debug( cpu, level0_ptr, (uint8_t*)&pointercount, sizeof(pointercount), 0 );
                if( pointercount > 0x1000000 || pointercount < 0 )
                    continue;

                cpu_memory_rw_debug( cpu, level0_ptr+0x8, (uint8_t*)&handlecount, sizeof(handlecount), 0 );
                if( handlecount > 0x1000 || handlecount < 0 )
                    continue;

                cpu_memory_rw_debug( cpu, level0_ptr+0x1a, (uint8_t*)&infomask, sizeof(infomask), 0 );
                if( infomask > 0x88 )
                    continue;

                cpu_memory_rw_debug( cpu, level0_ptr+0x18, (uint8_t*)&typeindex, sizeof(typeindex), 0 );
                if( typeindex >=50 || typeindex < 1 )
                    continue;                

                if( cpu_memory_rw_debug( cpu, level0_ptr+0x18, (uint8_t*)&typeindex, sizeof(typeindex), 0 ) != -1 ){
                    true_type =  (int)((typeindex ^ 0xc5 ^ ((level0_ptr & 0x0000ffffffffffff)>>8))& 0xff );
                    if( true_type == 32 ){
                        printf("0x%" PRIx64 "\t0x%" PRIx32, level0_ptr, i*4);
                        cpu_memory_rw_debug( cpu, level0_ptr + 0x88, (uint8_t*)&filelength, sizeof(filelength), 0 );
                        if (cpu_memory_rw_debug( cpu, level0_ptr + 0x90, (uint8_t*)&file_ptr, sizeof(file_ptr), 0 ) != -1 )
                            printf("\t0x%" PRIx64 , file_ptr);
                        else
                            continue;
                        if(file_ptr==0x0)
                            printf("\t\t");

                        printf("\t0x%" PRIx64 , grantedaccess);
                        if(grantedaccess == 0x0)
                            printf("\t");
                        printf("\tfile");

                        cpu_memory_rw_debug( cpu, level0_ptr+0x38, (uint8_t*)&DeviceObject_ptr, sizeof(DeviceObject_ptr), 0 );
                        if( cpu_memory_rw_debug( cpu, DeviceObject_ptr-0x30+0x1a, (uint8_t*)&device_infomask, sizeof(device_infomask), 0 ) != -1 ){
                            if(device_infomask & 0x2){
                                cpu_memory_rw_debug( cpu, DeviceObject_ptr-0x30-0x20+0x8, (uint8_t*)&device_filelength, sizeof(device_filelength), 0 );
                                cpu_memory_rw_debug( cpu, DeviceObject_ptr-0x30-0x20+0x10, (uint8_t*)&device_file_ptr, sizeof(device_file_ptr), 0 );
                                cpu_memory_rw_debug( cpu, device_file_ptr, (uint8_t*)device_file_buf, sizeof(device_file_buf), 0 );
                                
                                printf("\t{\\Device\\");
                                for( j=0 ; j<device_filelength ; j=j+1 )
                                    printf("%c", (char)(*(device_file_buf+j)));
                                cpu_memory_rw_debug( cpu, file_ptr, (uint8_t*)file_buf, sizeof(file_buf), 0 );
                                for( j=0 ; j<filelength ; j=j+1 )
                                    printf("%c", (char)(*(file_buf+j)));
                                printf( "}\n" );
                            }
                        }
                        else if (cpu_memory_rw_debug( cpu, file_ptr, (uint8_t*)file_buf, sizeof(file_buf), 0 ) != -1){
                            printf("\t{");
                            for( j=0 ; j<filelength ; j=j+1 )
                                printf("%c", (char)(*(file_buf+j)));
                            printf( "}\n" );
                        }
                    }
                    else{
                        switch(true_type){
                            case 2: printf("0x%" PRIx64 "\t0x%" PRIx32 "\t0x0  \t\t\t0x%" PRIx64, level0_ptr, i*4, grantedaccess); printf("      \tType\t"); break;
                            case 3: printf("0x%" PRIx64 "\t0x%" PRIx32 "\t0x0  \t\t\t0x%" PRIx64, level0_ptr, i*4, grantedaccess); printf("      \tDirectory\t"); break;
                            case 4: printf("0x%" PRIx64 "\t0x%" PRIx32 "\t0x0  \t\t\t0x%" PRIx64, level0_ptr, i*4, grantedaccess); printf("      \tSymbolicLink\t"); break;
                            case 5: printf("0x%" PRIx64 "\t0x%" PRIx32 "\t0x0  \t\t\t0x%" PRIx64, level0_ptr, i*4, grantedaccess); printf("      \tToken\t"); break;
                            case 6: printf("0x%" PRIx64 "\t0x%" PRIx32 "\t0x0  \t\t\t0x%" PRIx64, level0_ptr, i*4, grantedaccess); printf("      \tJob\t"); break;
                            case 7: printf("0x%" PRIx64 "\t0x%" PRIx32 "\t0x0  \t\t\t0x%" PRIx64, level0_ptr, i*4, grantedaccess); printf("      \tProcess\t"); break;
                            case 8: printf("0x%" PRIx64 "\t0x%" PRIx32 "\t0x0  \t\t\t0x%" PRIx64, level0_ptr, i*4, grantedaccess); printf("      \tThread\t"); break;
                            case 9: printf("0x%" PRIx64 "\t0x%" PRIx32 "\t0x0  \t\t\t0x%" PRIx64, level0_ptr, i*4, grantedaccess); printf("      \tUserApcReserve\t"); break;
                            case 10: printf("0x%" PRIx64 "\t0x%" PRIx32 "\t0x0  \t\t\t0x%" PRIx64, level0_ptr, i*4, grantedaccess); printf("      \tIoCompletionReserve\t"); break;
                            case 11: printf("0x%" PRIx64 "\t0x%" PRIx32 "\t0x0  \t\t\t0x%" PRIx64, level0_ptr, i*4, grantedaccess); printf("      \tSilo\t"); break;
                            case 12: printf("0x%" PRIx64 "\t0x%" PRIx32 "\t0x0  \t\t\t0x%" PRIx64, level0_ptr, i*4, grantedaccess); printf("      \tDebugObject\t"); break;
                            case 13: printf("0x%" PRIx64 "\t0x%" PRIx32 "\t0x0  \t\t\t0x%" PRIx64, level0_ptr, i*4, grantedaccess); printf("      \tEvent\t"); break;
                            case 14: printf("0x%" PRIx64 "\t0x%" PRIx32 "\t0x0  \t\t\t0x%" PRIx64, level0_ptr, i*4, grantedaccess); printf("      \tMutant\t"); break;
                            case 15: printf("0x%" PRIx64 "\t0x%" PRIx32 "\t0x0  \t\t\t0x%" PRIx64, level0_ptr, i*4, grantedaccess); printf("      \tCallback\t"); break;
                            case 16: printf("0x%" PRIx64 "\t0x%" PRIx32 "\t0x0  \t\t\t0x%" PRIx64, level0_ptr, i*4, grantedaccess); printf("      \tSemaphore\t"); break;
                            case 17: printf("0x%" PRIx64 "\t0x%" PRIx32 "\t0x0  \t\t\t0x%" PRIx64, level0_ptr, i*4, grantedaccess); printf("      \tTimer\t"); break;
                            case 18: printf("0x%" PRIx64 "\t0x%" PRIx32 "\t0x0  \t\t\t0x%" PRIx64, level0_ptr, i*4, grantedaccess); printf("      \tIRTimer\t"); break;
                            case 19: printf("0x%" PRIx64 "\t0x%" PRIx32 "\t0x0  \t\t\t0x%" PRIx64, level0_ptr, i*4, grantedaccess); printf("      \tProfile\t"); break;
                            case 20: printf("0x%" PRIx64 "\t0x%" PRIx32 "\t0x0  \t\t\t0x%" PRIx64, level0_ptr, i*4, grantedaccess); printf("      \tKeyedEvent\t"); break;
                            case 21: printf("0x%" PRIx64 "\t0x%" PRIx32 "\t0x0  \t\t\t0x%" PRIx64, level0_ptr, i*4, grantedaccess); printf("      \tWindowStation\t"); break;
                            case 22: printf("0x%" PRIx64 "\t0x%" PRIx32 "\t0x0  \t\t\t0x%" PRIx64, level0_ptr, i*4, grantedaccess); printf("      \tDesktop\t"); break;
                            case 23: printf("0x%" PRIx64 "\t0x%" PRIx32 "\t0x0  \t\t\t0x%" PRIx64, level0_ptr, i*4, grantedaccess); printf("      \tComposition\t"); break;
                            case 24: printf("0x%" PRIx64 "\t0x%" PRIx32 "\t0x0  \t\t\t0x%" PRIx64, level0_ptr, i*4, grantedaccess); printf("      \tRawInputManager\t"); break;
                            case 25: printf("0x%" PRIx64 "\t0x%" PRIx32 "\t0x0  \t\t\t0x%" PRIx64, level0_ptr, i*4, grantedaccess); printf("      \tTpWorkerFactory\t"); break;
                            case 26: printf("0x%" PRIx64 "\t0x%" PRIx32 "\t0x0  \t\t\t0x%" PRIx64, level0_ptr, i*4, grantedaccess); printf("      \tAdapter\t"); break;
                            case 27: printf("0x%" PRIx64 "\t0x%" PRIx32 "\t0x0  \t\t\t0x%" PRIx64, level0_ptr, i*4, grantedaccess); printf("      \tController\t"); break;
                            case 28: printf("0x%" PRIx64 "\t0x%" PRIx32 "\t0x0  \t\t\t0x%" PRIx64, level0_ptr, i*4, grantedaccess); printf("      \tDevice\t"); break;
                            case 29: printf("0x%" PRIx64 "\t0x%" PRIx32 "\t0x0  \t\t\t0x%" PRIx64, level0_ptr, i*4, grantedaccess); printf("      \tDriver\t"); break;
                            case 30: printf("0x%" PRIx64 "\t0x%" PRIx32 "\t0x0  \t\t\t0x%" PRIx64, level0_ptr, i*4, grantedaccess); printf("      \tIoCompletion\t"); break;
                            case 31: printf("0x%" PRIx64 "\t0x%" PRIx32 "\t0x0  \t\t\t0x%" PRIx64, level0_ptr, i*4, grantedaccess); printf("      \tWaitCompletionPacket\t"); break;
                            case 33: printf("0x%" PRIx64 "\t0x%" PRIx32 "\t0x0  \t\t\t0x%" PRIx64, level0_ptr, i*4, grantedaccess); printf("      \tTmTm\t"); break;
                            case 34: printf("0x%" PRIx64 "\t0x%" PRIx32 "\t0x0  \t\t\t0x%" PRIx64, level0_ptr, i*4, grantedaccess); printf("      \tTmTx\t"); break;
                            case 35: printf("0x%" PRIx64 "\t0x%" PRIx32 "\t0x0  \t\t\t0x%" PRIx64, level0_ptr, i*4, grantedaccess); printf("      \tTmRm\t"); break;
                            case 36: printf("0x%" PRIx64 "\t0x%" PRIx32 "\t0x0  \t\t\t0x%" PRIx64, level0_ptr, i*4, grantedaccess); printf("      \tTmEn\t"); break;
                            case 37: printf("0x%" PRIx64 "\t0x%" PRIx32 "\t0x0  \t\t\t0x%" PRIx64, level0_ptr, i*4, grantedaccess); printf("      \tSection\t"); break;
                            case 38: printf("0x%" PRIx64 "\t0x%" PRIx32 "\t0x0  \t\t\t0x%" PRIx64, level0_ptr, i*4, grantedaccess); printf("      \tSession\t"); break;
                            case 39: printf("0x%" PRIx64 "\t0x%" PRIx32 "\t0x0  \t\t\t0x%" PRIx64, level0_ptr, i*4, grantedaccess); printf("      \tPartition\t"); break;
                            case 40: printf("0x%" PRIx64 "\t0x%" PRIx32 "\t0x0  \t\t\t0x%" PRIx64, level0_ptr, i*4, grantedaccess); printf("      \tKey\t"); break;
                            case 41: printf("0x%" PRIx64 "\t0x%" PRIx32 "\t0x0  \t\t\t0x%" PRIx64, level0_ptr, i*4, grantedaccess); printf("      \tALPC Port\t"); break;
                            case 42: printf("0x%" PRIx64 "\t0x%" PRIx32 "\t0x0  \t\t\t0x%" PRIx64, level0_ptr, i*4, grantedaccess); printf("      \tPowerRequest\t"); break;
                            case 43: printf("0x%" PRIx64 "\t0x%" PRIx32 "\t0x0  \t\t\t0x%" PRIx64, level0_ptr, i*4, grantedaccess); printf("      \tWmiGuid\t"); break;
                            case 44: printf("0x%" PRIx64 "\t0x%" PRIx32 "\t0x0  \t\t\t0x%" PRIx64, level0_ptr, i*4, grantedaccess); printf("      \tEtwRegistration\t"); break;
                            case 45: printf("0x%" PRIx64 "\t0x%" PRIx32 "\t0x0  \t\t\t0x%" PRIx64, level0_ptr, i*4, grantedaccess); printf("      \tEtwConsumer\t"); break;
                            case 46: printf("0x%" PRIx64 "\t0x%" PRIx32 "\t0x0  \t\t\t0x%" PRIx64, level0_ptr, i*4, grantedaccess); printf("      \tDmaAdapter\t"); break;
                            case 47: printf("0x%" PRIx64 "\t0x%" PRIx32 "\t0x0  \t\t\t0x%" PRIx64, level0_ptr, i*4, grantedaccess); printf("      \tDmaDomain\t"); break;
                            case 48: printf("0x%" PRIx64 "\t0x%" PRIx32 "\t0x0  \t\t\t0x%" PRIx64, level0_ptr, i*4, grantedaccess); printf("      \tPcwObject\t"); break;
                            //
                            case 49: printf("0x%" PRIx64 "\t0x%" PRIx32 "\t0x0  \t\t\t0x%" PRIx64, level0_ptr, i*4, grantedaccess); printf("      \tFilterConnectionPort\t"); break;
                            case 50: printf("0x%" PRIx64 "\t0x%" PRIx32 "\t0x0  \t\t\t0x%" PRIx64, level0_ptr, i*4, grantedaccess); printf("      \tFilterCommunicationPort\t"); break;
                            case 51: printf("0x%" PRIx64 "\t0x%" PRIx32 "\t0x0  \t\t\t0x%" PRIx64, level0_ptr, i*4, grantedaccess); printf("      \tNetworkNamespace\t"); break;
                            case 52: printf("0x%" PRIx64 "\t0x%" PRIx32 "\t0x0  \t\t\t0x%" PRIx64, level0_ptr, i*4, grantedaccess); printf("      \tDxgkSharedResource\t"); break;
                            case 53: printf("0x%" PRIx64 "\t0x%" PRIx32 "\t0x0  \t\t\t0x%" PRIx64, level0_ptr, i*4, grantedaccess); printf("      \tDxgkSharedSyncObject\t"); break;
                            case 54: printf("0x%" PRIx64 "\t0x%" PRIx32 "\t0x0  \t\t\t0x%" PRIx64, level0_ptr, i*4, grantedaccess); printf("      \tDxgkSharedSwapChainObject\t"); break;
                        }

                        if( true_type == 8 ){
                            if( cpu_memory_rw_debug( cpu, level0_ptr+0x30+0x628, (uint8_t*)&uniqueProcess, sizeof(uniqueProcess), 0 ) != -1 )
                                printf("{TID %d}", uniqueProcess);
                            if( cpu_memory_rw_debug( cpu, level0_ptr+0x30+0x628+0x8, (uint8_t*)&uniqueThread, sizeof(uniqueThread), 0 ) != -1 )
                                printf("{PID %d}", uniqueThread);
                        }
                        else if( true_type == 40 ){
                            printf("0{");
                            cpu_memory_rw_debug( cpu, level0_ptr+0x30+0x8, (uint8_t*)&kcb_ptr, sizeof(kcb_ptr), 0 );
                            cpu_memory_rw_debug( cpu, kcb_ptr+0x40, (uint8_t*)&parent_kcb_ptr, sizeof(parent_kcb_ptr), 0 );
printf(" 0x%" PRIx64 " ", kcb_ptr);
printf("###");
printf(" 0x%" PRIx64 " ", parent_kcb_ptr);
cpu_memory_rw_debug( cpu, kcb_ptr+0x48, (uint8_t*)&nameblock_ptr, sizeof(nameblock_ptr), 0 );
cpu_memory_rw_debug( cpu, nameblock_ptr+0x1a, (uint8_t*)&key_name_buf, 6, 0 );
printf("$$$");
printf(" 0x%" PRIx64 " ", nameblock_ptr);
                                cpu_memory_rw_debug( cpu, nameblock_ptr+0x18, (uint8_t*)&key_name_length, sizeof(key_name_length), 0 );
printf("&&&");
printf(" %d", key_name_length);

                            while( cpu_memory_rw_debug( cpu, parent_kcb_ptr, (uint8_t*)&check_ptr, sizeof(check_ptr), 0 ) != -1 ){
                                if( cpu_memory_rw_debug( cpu, kcb_ptr+0x48, (uint8_t*)&nameblock_ptr, sizeof(nameblock_ptr), 0 ) == -1 || cpu_memory_rw_debug( cpu, nameblock_ptr+0x1a, (uint8_t*)&key_name_buf, 6, 0 ) == -1 )
                                        break;
                                cpu_memory_rw_debug( cpu, nameblock_ptr+0x18, (uint8_t*)&key_name_length, sizeof(key_name_length), 0 );
                                for( j=0 ; j<key_name_length ; j=j+1 )
                                    printf("%c", (char)(*(key_name_buf+j)));
                                printf("\t");
                                kcb_ptr = parent_kcb_ptr;
                                cpu_memory_rw_debug( cpu, kcb_ptr+0x40, (uint8_t*)&parent_kcb_ptr, sizeof(parent_kcb_ptr), 0 );
                            }
                            printf("}");
                        }

                       else if( infomask & 0x2 && infomask & 0x1 )
                        {
                            if( cpu_memory_rw_debug( cpu, level0_ptr-0x20-0x20+0x8, (uint8_t*)&nameinfo_length, sizeof(nameinfo_length), 0 ) != -1 )
                            if( cpu_memory_rw_debug( cpu, level0_ptr-0x20-0x20+0x10, (uint8_t*)&nameinfo_ptr, sizeof(nameinfo_ptr), 0 ) != -1 )
                            if( cpu_memory_rw_debug( cpu, nameinfo_ptr, (uint8_t*)nameinfo_buf, sizeof(nameinfo_buf), 0 ) != -1 )
                            {
                                printf("{");
                                for( j=0 ; j<nameinfo_length ; j=j+1 )
                                    printf("%c", (char)(*(nameinfo_buf+j)));
                                printf("}\n");
                            }
                        }

                       else  if( infomask & 0x2 )
                        {
                            if( cpu_memory_rw_debug( cpu, level0_ptr-0x20+0x8, (uint8_t*)&nameinfo_length, sizeof(nameinfo_length), 0 ) != -1 )
                            if( cpu_memory_rw_debug( cpu, level0_ptr-0x20+0x10, (uint8_t*)&nameinfo_ptr, sizeof(nameinfo_ptr), 0 ) != -1 )
                            if( cpu_memory_rw_debug( cpu, nameinfo_ptr, (uint8_t*)nameinfo_buf, sizeof(nameinfo_buf), 0 ) != -1 )
                            {
                                printf("{");
                                for( j=0 ; j<nameinfo_length ; j=j+1 )
                                    printf("%c", (char)(*(nameinfo_buf+j)));
                                printf("}\n");
                            }
                        }
                        printf("\n");
                    }
                }
            }
        }
        else if ( table_level == 1 ){
            for( k=0 ; k<512 ; k=k+1 ){
                tablecode_ptr = tablecode_ptr+0x8;
                cpu_memory_rw_debug( cpu, tablecode_ptr, (uint8_t*)&level1_ptr, sizeof(level1_ptr), 0 );
                if (cpu_memory_rw_debug( cpu, level1_ptr, (uint8_t*)&level0_ptr, sizeof(level0_ptr), 0 ) != -1){

                    for( i=0 ; i<256 ; i=i+1 ){
                        level1_ptr = level1_ptr+0x10;
                        cpu_memory_rw_debug( cpu, level1_ptr, (uint8_t*)&level0_ptr, sizeof(level0_ptr), 0 );
                        level0_ptr = ( ( (level0_ptr & 0xfffffffffff00000) >> 16 ) + 0xffff000000000000);
                        cpu_memory_rw_debug( cpu, tablecode_ptr+0x8, (uint8_t*)&grantedaccess, sizeof(grantedaccess), 0 );
                        grantedaccess = grantedaccess & 0x0000000001ffffff;
        
                        cpu_memory_rw_debug( cpu, level0_ptr, (uint8_t*)&pointercount, sizeof(pointercount), 0 );
                        if( pointercount > 0x01000000 || pointercount < 0 )
                            continue;
        
                        cpu_memory_rw_debug( cpu, level0_ptr+0x1a, (uint8_t*)&infomask, sizeof(infomask), 0 );
                        if( infomask > 0x88 )
                            continue;
                        
                        if( cpu_memory_rw_debug( cpu, level0_ptr+0x18, (uint8_t*)&typeindex, sizeof(typeindex), 0 ) != -1 ){
                            true_type =  (int)((typeindex ^ 0xc5 ^ ((level0_ptr & 0x0000ffffffffffff)>>8))& 0xff );
                            if( true_type == 32 ){
                                printf("0x%" PRIx64 "\t0x%" PRIx32, level0_ptr, i*4+k*1024);
                                cpu_memory_rw_debug( cpu, level0_ptr + 0x88, (uint8_t*)&filelength, sizeof(filelength), 0 );
                                if (cpu_memory_rw_debug( cpu, level0_ptr + 0x90, (uint8_t*)&file_ptr, sizeof(file_ptr), 0 ) != -1 )
                                   printf("\t0x%" PRIx64 , file_ptr);
                                else
                                    continue;
                                if(file_ptr==0x0)
                                    printf("\t\t");
        
                                printf("\t0x%" PRIx64 , grantedaccess);
                                if(grantedaccess == 0x0)
                                    printf("\t");
                                printf("\tfile");

                                cpu_memory_rw_debug( cpu, level0_ptr+0x38, (uint8_t*)&DeviceObject_ptr, sizeof(DeviceObject_ptr), 0 );
                                if( cpu_memory_rw_debug( cpu, DeviceObject_ptr-0x30+0x1a, (uint8_t*)&device_infomask, sizeof(device_infomask), 0 ) != -1 ){
                                    if(device_infomask & 0x2){
                                        cpu_memory_rw_debug( cpu, DeviceObject_ptr-0x30-0x20+0x8, (uint8_t*)&device_filelength, sizeof(device_filelength), 0 );
                                        cpu_memory_rw_debug( cpu, DeviceObject_ptr-0x30-0x20+0x10, (uint8_t*)&device_file_ptr, sizeof(device_file_ptr), 0 );
                                        cpu_memory_rw_debug( cpu, device_file_ptr, (uint8_t*)device_file_buf, sizeof(device_file_buf), 0 );
                                        printf("\t{\\Device\\");
                                        for( j=0 ; j<device_filelength ; j=j+1 )
                                            printf("%c", (char)(*(device_file_buf+j)));
                                        cpu_memory_rw_debug( cpu, file_ptr, (uint8_t*)file_buf, sizeof(file_buf), 0 );
                                        for( j=0 ; j<filelength ; j=j+1 )
                                            printf("%c", (char)(*(file_buf+j)));
                                        printf( "}\n" );
                                    }
                                }
                                else if (cpu_memory_rw_debug( cpu, file_ptr, (uint8_t*)file_buf, sizeof(file_buf), 0 ) != -1){
                                    printf("\t{");
                                    for( j=0 ; j<filelength ; j=j+1 )
                                        printf("%c", (char)(*(file_buf+j)));
                                    printf( "}\n" );
                                }
                            }
                    else{
                        switch(true_type){
                            case 2: printf("0x%" PRIx64 "\t0x%" PRIx32 "\t0x0  \t\t\t0x%" PRIx64, level0_ptr, i*4+k*1024, grantedaccess); printf("      \tType\t"); break;
                            case 3: printf("0x%" PRIx64 "\t0x%" PRIx32 "\t0x0  \t\t\t0x%" PRIx64, level0_ptr, i*4+k*1024, grantedaccess); printf("      \tDirectory\t"); break;
                            case 4: printf("0x%" PRIx64 "\t0x%" PRIx32 "\t0x0  \t\t\t0x%" PRIx64, level0_ptr, i*4+k*1024, grantedaccess); printf("      \tSymbolicLink\t"); break;
                            case 5: printf("0x%" PRIx64 "\t0x%" PRIx32 "\t0x0  \t\t\t0x%" PRIx64, level0_ptr, i*4+k*1024, grantedaccess); printf("      \tToken\t"); break;
                            case 6: printf("0x%" PRIx64 "\t0x%" PRIx32 "\t0x0  \t\t\t0x%" PRIx64, level0_ptr, i*4+k*1024, grantedaccess); printf("      \tJob\t"); break;
                            case 7: printf("0x%" PRIx64 "\t0x%" PRIx32 "\t0x0  \t\t\t0x%" PRIx64, level0_ptr, i*4+k*1024, grantedaccess); printf("      \tProcess\t"); break;
                            case 8: printf("0x%" PRIx64 "\t0x%" PRIx32 "\t0x0  \t\t\t0x%" PRIx64, level0_ptr, i*4+k*1024, grantedaccess); printf("      \tThread\t"); break;
                            case 9: printf("0x%" PRIx64 "\t0x%" PRIx32 "\t0x0  \t\t\t0x%" PRIx64, level0_ptr, i*4+k*1024, grantedaccess); printf("      \tUserApcReserve\t"); break;
                            case 10: printf("0x%" PRIx64 "\t0x%" PRIx32 "\t0x0  \t\t\t0x%" PRIx64, level0_ptr, i*4+k*1024, grantedaccess); printf("      \tIoCompletionReserve\t"); break;
                            case 11: printf("0x%" PRIx64 "\t0x%" PRIx32 "\t0x0  \t\t\t0x%" PRIx64, level0_ptr, i*4+k*1024, grantedaccess); printf("      \tSilo\t"); break;
                            case 12: printf("0x%" PRIx64 "\t0x%" PRIx32 "\t0x0  \t\t\t0x%" PRIx64, level0_ptr, i*4+k*1024, grantedaccess); printf("      \tDebugObject\t"); break;
                            case 13: printf("0x%" PRIx64 "\t0x%" PRIx32 "\t0x0  \t\t\t0x%" PRIx64, level0_ptr, i*4+k*1024, grantedaccess); printf("      \tEvent\t"); break;
                            case 14: printf("0x%" PRIx64 "\t0x%" PRIx32 "\t0x0  \t\t\t0x%" PRIx64, level0_ptr, i*4+k*1024, grantedaccess); printf("      \tMutant\t"); break;
                            case 15: printf("0x%" PRIx64 "\t0x%" PRIx32 "\t0x0  \t\t\t0x%" PRIx64, level0_ptr, i*4+k*1024, grantedaccess); printf("      \tCallback\t"); break;
                            case 16: printf("0x%" PRIx64 "\t0x%" PRIx32 "\t0x0  \t\t\t0x%" PRIx64, level0_ptr, i*4+k*1024, grantedaccess); printf("      \tSemaphore\t"); break;
                            case 17: printf("0x%" PRIx64 "\t0x%" PRIx32 "\t0x0  \t\t\t0x%" PRIx64, level0_ptr, i*4+k*1024, grantedaccess); printf("      \tTimer\t"); break;
                            case 18: printf("0x%" PRIx64 "\t0x%" PRIx32 "\t0x0  \t\t\t0x%" PRIx64, level0_ptr, i*4+k*1024, grantedaccess); printf("      \tIRTimer\t"); break;
                            case 19: printf("0x%" PRIx64 "\t0x%" PRIx32 "\t0x0  \t\t\t0x%" PRIx64, level0_ptr, i*4+k*1024, grantedaccess); printf("      \tProfile\t"); break;
                            case 20: printf("0x%" PRIx64 "\t0x%" PRIx32 "\t0x0  \t\t\t0x%" PRIx64, level0_ptr, i*4+k*1024, grantedaccess); printf("      \tKeyedEvent\t"); break;
                            case 21: printf("0x%" PRIx64 "\t0x%" PRIx32 "\t0x0  \t\t\t0x%" PRIx64, level0_ptr, i*4+k*1024, grantedaccess); printf("      \tWindowStation\t"); break;
                            case 22: printf("0x%" PRIx64 "\t0x%" PRIx32 "\t0x0  \t\t\t0x%" PRIx64, level0_ptr, i*4+k*1024, grantedaccess); printf("      \tDesktop\t"); break;
                            case 23: printf("0x%" PRIx64 "\t0x%" PRIx32 "\t0x0  \t\t\t0x%" PRIx64, level0_ptr, i*4+k*1024, grantedaccess); printf("      \tComposition\t"); break;
                            case 24: printf("0x%" PRIx64 "\t0x%" PRIx32 "\t0x0  \t\t\t0x%" PRIx64, level0_ptr, i*4+k*1024, grantedaccess); printf("      \tRawInputManager\t"); break;
                            case 25: printf("0x%" PRIx64 "\t0x%" PRIx32 "\t0x0  \t\t\t0x%" PRIx64, level0_ptr, i*4+k*1024, grantedaccess); printf("      \tTpWorkerFactory\t"); break;
                            case 26: printf("0x%" PRIx64 "\t0x%" PRIx32 "\t0x0  \t\t\t0x%" PRIx64, level0_ptr, i*4+k*1024, grantedaccess); printf("      \tAdapter\t"); break;
                            case 27: printf("0x%" PRIx64 "\t0x%" PRIx32 "\t0x0  \t\t\t0x%" PRIx64, level0_ptr, i*4+k*1024, grantedaccess); printf("      \tController\t"); break;
                            case 28: printf("0x%" PRIx64 "\t0x%" PRIx32 "\t0x0  \t\t\t0x%" PRIx64, level0_ptr, i*4+k*1024, grantedaccess); printf("      \tDevice\t"); break;
                            case 29: printf("0x%" PRIx64 "\t0x%" PRIx32 "\t0x0  \t\t\t0x%" PRIx64, level0_ptr, i*4+k*1024, grantedaccess); printf("      \tDriver\t"); break;
                            case 30: printf("0x%" PRIx64 "\t0x%" PRIx32 "\t0x0  \t\t\t0x%" PRIx64, level0_ptr, i*4+k*1024, grantedaccess); printf("      \tIoCompletion\t"); break;
                            case 31: printf("0x%" PRIx64 "\t0x%" PRIx32 "\t0x0  \t\t\t0x%" PRIx64, level0_ptr, i*4+k*1024, grantedaccess); printf("      \tWaitCompletionPacket\t"); break;
                            case 33: printf("0x%" PRIx64 "\t0x%" PRIx32 "\t0x0  \t\t\t0x%" PRIx64, level0_ptr, i*4+k*1024, grantedaccess); printf("      \tTmTm\t"); break;
                            case 34: printf("0x%" PRIx64 "\t0x%" PRIx32 "\t0x0  \t\t\t0x%" PRIx64, level0_ptr, i*4+k*1024, grantedaccess); printf("      \tTmTx\t"); break;
                            case 35: printf("0x%" PRIx64 "\t0x%" PRIx32 "\t0x0  \t\t\t0x%" PRIx64, level0_ptr, i*4+k*1024, grantedaccess); printf("      \tTmRm\t"); break;
                            case 36: printf("0x%" PRIx64 "\t0x%" PRIx32 "\t0x0  \t\t\t0x%" PRIx64, level0_ptr, i*4+k*1024, grantedaccess); printf("      \tTmEn\t"); break;
                            case 37: printf("0x%" PRIx64 "\t0x%" PRIx32 "\t0x0  \t\t\t0x%" PRIx64, level0_ptr, i*4+k*1024, grantedaccess); printf("      \tSection\t"); break;
                            case 38: printf("0x%" PRIx64 "\t0x%" PRIx32 "\t0x0  \t\t\t0x%" PRIx64, level0_ptr, i*4+k*1024, grantedaccess); printf("      \tSession\t"); break;
                            case 39: printf("0x%" PRIx64 "\t0x%" PRIx32 "\t0x0  \t\t\t0x%" PRIx64, level0_ptr, i*4+k*1024, grantedaccess); printf("      \tPartition\t"); break;
                            case 40: printf("0x%" PRIx64 "\t0x%" PRIx32 "\t0x0  \t\t\t0x%" PRIx64, level0_ptr, i*4+k*1024, grantedaccess); printf("      \tKey\t"); break;
                            case 41: printf("0x%" PRIx64 "\t0x%" PRIx32 "\t0x0  \t\t\t0x%" PRIx64, level0_ptr, i*4+k*1024, grantedaccess); printf("      \tALPC Port\t"); break;
                            case 42: printf("0x%" PRIx64 "\t0x%" PRIx32 "\t0x0  \t\t\t0x%" PRIx64, level0_ptr, i*4+k*1024, grantedaccess); printf("      \tPowerRequest\t"); break;
                            case 43: printf("0x%" PRIx64 "\t0x%" PRIx32 "\t0x0  \t\t\t0x%" PRIx64, level0_ptr, i*4+k*1024, grantedaccess); printf("      \tWmiGuid\t"); break;
                            case 44: printf("0x%" PRIx64 "\t0x%" PRIx32 "\t0x0  \t\t\t0x%" PRIx64, level0_ptr, i*4+k*1024, grantedaccess); printf("      \tEtwRegistration\t"); break;
                            case 45: printf("0x%" PRIx64 "\t0x%" PRIx32 "\t0x0  \t\t\t0x%" PRIx64, level0_ptr, i*4+k*1024, grantedaccess); printf("      \tEtwConsumer\t"); break;
                            case 46: printf("0x%" PRIx64 "\t0x%" PRIx32 "\t0x0  \t\t\t0x%" PRIx64, level0_ptr, i*4+k*1024, grantedaccess); printf("      \tDmaAdapter\t"); break;
                            case 47: printf("0x%" PRIx64 "\t0x%" PRIx32 "\t0x0  \t\t\t0x%" PRIx64, level0_ptr, i*4+k*1024, grantedaccess); printf("      \tDmaDomain\t"); break;
                            case 48: printf("0x%" PRIx64 "\t0x%" PRIx32 "\t0x0  \t\t\t0x%" PRIx64, level0_ptr, i*4+k*1024, grantedaccess); printf("      \tPcwObject\t"); break;
                            //
                            case 49: printf("0x%" PRIx64 "\t0x%" PRIx32 "\t0x0  \t\t\t0x%" PRIx64, level0_ptr, i*4+k*1024, grantedaccess); printf("      \tFilterConnectionPort\t"); break;
                            case 50: printf("0x%" PRIx64 "\t0x%" PRIx32 "\t0x0  \t\t\t0x%" PRIx64, level0_ptr, i*4+k*1024, grantedaccess); printf("      \tFilterCommunicationPort\t"); break;
                            case 51: printf("0x%" PRIx64 "\t0x%" PRIx32 "\t0x0  \t\t\t0x%" PRIx64, level0_ptr, i*4+k*1024, grantedaccess); printf("      \tNetworkNamespace\t"); break;
                            case 52: printf("0x%" PRIx64 "\t0x%" PRIx32 "\t0x0  \t\t\t0x%" PRIx64, level0_ptr, i*4+k*1024, grantedaccess); printf("      \tDxgkSharedResource\t"); break;
                            case 53: printf("0x%" PRIx64 "\t0x%" PRIx32 "\t0x0  \t\t\t0x%" PRIx64, level0_ptr, i*4+k*1024, grantedaccess); printf("      \tDxgkSharedSyncObject\t"); break;
                            case 54: printf("0x%" PRIx64 "\t0x%" PRIx32 "\t0x0  \t\t\t0x%" PRIx64, level0_ptr, i*4+k*1024, grantedaccess); printf("      \tDxgkSharedSwapChainObject\t"); break;
                        }
                        if( true_type == 8 ){
                            if( cpu_memory_rw_debug( cpu, level0_ptr+0x30+0x628, (uint8_t*)&uniqueProcess, sizeof(uniqueProcess), 0 ) != -1 ){
                                printf("{TID %d}", uniqueProcess);
                            }
                            if( cpu_memory_rw_debug( cpu, level0_ptr+0x30+0x628+0x8, (uint8_t*)&uniqueThread, sizeof(uniqueThread), 0 ) != -1 ){
                                printf("{PID %d}", uniqueThread);
                            }
                        }
                        else if( true_type == 40 ){
                            printf("1{");
                            cpu_memory_rw_debug( cpu, level0_ptr+0x30+0x8, (uint8_t*)&kcb_ptr, sizeof(kcb_ptr), 0 );
                            cpu_memory_rw_debug( cpu, kcb_ptr+0x40, (uint8_t*)&parent_kcb_ptr, sizeof(parent_kcb_ptr), 0 );
                            while( cpu_memory_rw_debug( cpu, parent_kcb_ptr, (uint8_t*)&check_ptr, sizeof(check_ptr), 0 ) != -1 ){
                                if( cpu_memory_rw_debug( cpu, kcb_ptr+0x48, (uint8_t*)&nameblock_ptr, sizeof(nameblock_ptr), 0 ) ==-1 || cpu_memory_rw_debug( cpu, nameblock_ptr+0x1a, (uint8_t*)&key_name_buf, 6, 0 ) == -1 )
                                        break;

                                for( j=0 ; j<6 ; j=j+1 )
                                    printf("%c", (char)(*(key_name_buf+j)));
                                printf("\t");
                                kcb_ptr = parent_kcb_ptr;
                                cpu_memory_rw_debug( cpu, kcb_ptr+0x40, (uint8_t*)&parent_kcb_ptr, sizeof(parent_kcb_ptr), 0 );
                            }
                            printf("}");
                        }

                       else if( infomask & 0x2 && infomask & 0x1 )
                        {
                            if( cpu_memory_rw_debug( cpu, level0_ptr-0x20-0x20+0x8, (uint8_t*)&nameinfo_length, sizeof(nameinfo_length), 0 ) != -1 )
                            if( cpu_memory_rw_debug( cpu, level0_ptr-0x20-0x20+0x10, (uint8_t*)&nameinfo_ptr, sizeof(nameinfo_ptr), 0 ) != -1 )
                            if( cpu_memory_rw_debug( cpu, nameinfo_ptr, (uint8_t*)nameinfo_buf, sizeof(nameinfo_buf), 0 ) != -1 )
                            {
                                printf("{");
                                for( j=0 ; j<nameinfo_length ; j=j+1 )
                                    printf("%c", (char)(*(nameinfo_buf+j)));
                                printf("}\n");
                            }
                        }

                        else if ( infomask & 0x2 )
                        if ( cpu_memory_rw_debug( cpu, level0_ptr-0x20+0x8, (uint8_t*)&nameinfo_length, sizeof(nameinfo_length), 0 ) != -1 )
                        if ( cpu_memory_rw_debug( cpu, level0_ptr-0x20+0x10, (uint8_t*)&nameinfo_ptr, sizeof(nameinfo_ptr), 0 ) != -1 )
                        if ( cpu_memory_rw_debug( cpu, nameinfo_ptr, (uint8_t*)nameinfo_buf, sizeof(nameinfo_buf), 0 ) != -1 )
                        {
                            printf("{");
                            for( j=0 ; j<nameinfo_length ; j=j+1 )
                                printf("%c", (char)(*(nameinfo_buf+j)));
                            printf("}\n");
                        }
                        printf("\n");
                    }

                        }
                    }

                }
            }
        }
        else {
            // [TODO]
            printf("[Table_level] 2\n");
        }


            x86_cpu->cr[3] = cr3;
            cpu_memory_rw_debug( cpu, eprocess_ptr + offset_peb_to_eprocess, (uint8_t*)&peb_ptr, sizeof(peb_ptr), 0 );
            if(cpu_memory_rw_debug( cpu, peb_ptr + offset_parameter_to_peb, (uint8_t*)&rtl_ptr, sizeof(rtl_ptr), 0 )==-1)
                rtl_ptr=0;

            if( cpu_memory_rw_debug( cpu, rtl_ptr + offset_length_to_rtl, (uint8_t*)&length, sizeof(length), 0 ) ==-1)
                length=0;

            if(cpu_memory_rw_debug( cpu, rtl_ptr + offset_buffer_to_rtl, (uint8_t*)&buf_ptr, sizeof(buf_ptr), 0 )==-1)
                buf_ptr=0;

            printf( "Full path: " );
            if(cpu_memory_rw_debug( cpu, buf_ptr, (uint8_t*)buf, sizeof(buf), 0 )!=-1)
            {
                for(i=0;i<length;i=i+1)
                    printf("%c", (char)(*(buf+i)));
            }

            printf( "\n\n" );
            x86_cpu->cr[3] = cr3_init;
            fflush(stdout);
        }

        // read next entry
        cpu_memory_rw_debug( cpu, eprocess_ptr + offset_blink_to_eprocess, (uint8_t*)&eprocess_ptr, sizeof(eprocess_ptr), 0 );
        // Substract entry_list offset to find base address of eprocess
        eprocess_ptr -= offset_entry_list_to_eprocess;
       
        count++;
        if(count > 1000)
            break;
    } while( eprocess_ptr != eprocess_ptr_init );

    return 0;
}

char* parse_unicode_strptr(uint64_t ustr_ptr, CPUState *cpu)
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
        return NULL;

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
    free(buf);
    return str;
}

char* parse_unicode_str(uint8_t* ustr, CPUState *cpu)
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
        return NULL;

    f_info = memfrs_q_field(justr, "Buffer");
    offset = f_info->offset;
    buf_ptr = *((uint64_t*)(ustr+offset));
    memfrs_close_field(f_info);
    

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
    free(buf);
    return str;
}

/*******************************************************************
int memfrs_load_structs( const char* gvar_filename)

Load the global variable information into g_globalvar_info.

INPUT:     const char* gvar_filename,  the filename of json global variable database
OUTPUT:    int,                        return 0 if sucess, and not 0 otherwise

*******************************************************************/
int memfrs_load_globalvar( const char* gvar_filename)
{
    g_globalvar_info = json_object_from_file(gvar_filename);
    return 0;
}

/*******************************************************************
json_object* memfrs_q_globalvar(const char* gvar_name)

query the global variable's info via given variable name

INPUT:    const char* gvar_name,  the name of interesting global symbol
OUTPUT:   json_object*,         json object representation of the target global var

*******************************************************************/
json_object* memfrs_q_globalvar(const char* gvar_name)
{
    json_object* target = NULL;

    if(g_globalvar_info==NULL)
        return NULL;

    // Query global structure info with structure name ds_name
    // Restore the query result into target json_object 
    json_object_object_get_ex(g_globalvar_info, gvar_name, &target);

    if(target==NULL)
        printf("%s not found\n", gvar_name);
    return target;
}

/*******************************************************************
uint64_t memfrs_gvar_offset(json_object* gvarobj)

Get the virtual address of specific global variable, which is in
json_object format. 

memfrs_q_globalvar should be invoked first to get the json_object.

INPUT:    json_object* gvarobj  the json obj of interesting global symbol
OUTPUT:   int64_t               the virtual address of specific global variable, -1 indicates fails

*******************************************************************/
int64_t memfrs_gvar_offset(json_object* gvarobj)
{
    if(gvarobj==NULL)
        return -1;
    json_object* tmp_jobject = json_object_array_get_idx(gvarobj, 0);
    uint64_t offset = json_object_get_int(tmp_jobject);
    return offset;
}

/*
typedef struct reverse_symbol {
    int offset;            // we'll use this field as the key //
    char* symbol;             
    UT_hash_handle hh; // makes this structure hashable //
} reverse_symbol;*/

reverse_symbol* memfrs_build_gvar_lookup_map(void)
{
    //json_object* lookup_map = NULL;
    // Check if kernel base and global var exist
    uint64_t kernel_base = memfrs_get_nt_kernel_base();
    if( kernel_base ==0 ){
        printf("Kernel not found\n");
        return NULL;
    }
    if( g_globalvar_info==NULL ){
        printf("gvar information not found\n");
        return NULL;
    }
    
    //lookup_map = json_object_new_object();
    reverse_symbol *rev_symtab = NULL; 
    json_object_object_foreach( g_globalvar_info, key, val){
        json_object* tmp_jobject = json_object_array_get_idx(val, 0);
        uint64_t offset = json_object_get_int(tmp_jobject);
        reverse_symbol* rec = (reverse_symbol*)malloc(sizeof(reverse_symbol)); 
        rec->offset = offset;
        rec->symbol = key;
        HASH_ADD_INT( rev_symtab, offset, rec ); 
    }
    return rev_symtab;
}

char* memfrs_get_symbolname_via_address(reverse_symbol* rsym_tab, int offset)
{
    reverse_symbol* sym = NULL;

    if(rsym_tab == NULL)
        return NULL;

    HASH_FIND_INT(rsym_tab, &offset, sym);

    if(sym == NULL)
        return NULL;
    return sym->symbol; 
}

int memfrs_free_reverse_lookup_map(reverse_symbol* rsym_tab)
{
    reverse_symbol *current_sym, *tmp;

    if(rsym_tab == NULL)
        return -1;

    HASH_ITER(hh, rsym_tab, current_sym, tmp){
        HASH_DEL(rsym_tab, current_sym);
        free(current_sym);
    }
    return 0;
}

int memfrs_display_type(CPUState *cpu, uint64_t addr, const char* struct_name)
{
    printf("%s\n", struct_name);
    json_object* jobj = memfrs_q_struct(struct_name);    
    uint8_t buf[0x30];
    char type_info[64];
    if(jobj==NULL)
        return -1;
    json_object_object_foreach(jobj, key, val){
        //printf("%s: %s\n", key, json_object_to_json_string(val));
        if( strcmp(key, "[structure_size]")==0)
            continue;
        json_object* type = json_object_array_get_idx(val, 0);
        int offset = json_object_get_int(json_object_array_get_idx(val, 1));
        int size = json_object_get_int(json_object_array_get_idx(val, 2));
  
        strncpy(type_info, key, 63);
        strncat(type_info, "(", 63);
        strncat(type_info, json_object_get_string(type), 63);
        strncat(type_info, ")", 63);

        printf("%-30s@%lx:\t", type_info, addr+offset);
        int i;
        if(size > 0x10)
        {
            printf("...\n");
            continue;
        }
        cpu_memory_rw_debug( cpu, addr+offset , buf, 0x30, 0 );
        for(i=0 ; i< size; i++)
        {
            printf("%02x ", buf[i]);
        }
        
        printf("\n");
    }
    return 0;
}

int memfrs_get_virmem_struct_content(
        CPUState   *cpu,
        uint64_t    cr3,
        uint8_t    *buffer,
        int         len,
        uint64_t    struct_addr,
        const char *struct_type_name,
        int         depth,
        ...) {
    // XXX: Now use extra char at beginning of field name to
    // indicate that the field is a pointer or not.
    // Should load and parse the structure file correctly instead.
    // XXX: assuming pointer has size of 8
    json_object *struct_type;
    va_list vl;
    field_info *info = NULL;
    int errcode;
    const char *field_name;

    struct_type = memfrs_q_struct(struct_type_name);
    if (struct_type == NULL)
        return -1;

    va_start(vl, depth);
    // Process field query
    while (depth--) {
        if (info && info->is_pointer) {
            errcode = memfrs_get_virmem_content(cpu, cr3, struct_addr, 8, (uint8_t*)&struct_addr);
            if (errcode == -1)
                goto FAIL;
        }

        free(info);
        field_name = va_arg(vl, const char*);
        info = memfrs_q_field(struct_type, field_name+1);
        if (info == NULL)
            goto FAIL;

        if (field_name[0] == '*') {
            field_name++;
            info->is_pointer = true;
        }
        struct_addr += info->offset;
        struct_type = info->jobject_type;
    }
    free(info);
    va_end(vl);

    return memfrs_get_virmem_content(cpu, cr3, struct_addr, len, buffer);
FAIL:
    va_end(vl);
    return -1;
}
