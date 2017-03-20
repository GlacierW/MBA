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
#include "monitor/monitor.h"
#include "include/exec/cpu-common.h"
#include "exec/cpu-all.h"
#include "include/utarray.h"
#include "include/uthash.h"
#include "json-c/json.h"

#include "memfrs.h"
#include "memfrs-priv.h"
#include<stdlib.h>

#include "handles.h"

#define _OBJECT_HEADER_CREATOR_INFO 0x20
#define _OBJECT_HEADER_NAME_INFO 0x20
#define _OBJECT_HEADER_HANDLE_INFO 0x10
#define _OBJECT_HEADER_QUOTA_INFO 0x20
#define _OBJECT_HEADER_PROCESS_INFO 0x10
#define _OBJECT_HEADER_AUDIT_INFO 0x10
#define _OBJECT_HEADER_PADDING_INFO 0x4

static int g_body_to_object_header;



/*******************************************************************
static void add_handle_field_to_struct(UT_array *handle_node, int entry_index, uint64_t handle_table_entry_ptr, uint64_t grantedaccess, char* true_type, char* detail)

add a handle entry data in UT_array structure process

INPUT:     UT_array *handle_node                target handles_node structure
           int entry_index,                     entry table index
           uint64_t handle_table_entry_ptr,     address of handle table entey 
           uint64_t grantedaccess,              granted access
           char* true_type,                     true type of handle table entry
           char* detail                         details of handles table entry
OUTPUT:    void
*******************************************************************/
static void add_handle_field_to_struct(UT_array *handle_node, int entry_index, uint64_t handle_table_entry_ptr, uint64_t grantedaccess, char* true_type, char* detail)
{
    handles_node_st handle_node_data;

    // entry_index
    handle_node_data.handle_table_entry_index = entry_index;

    // handle_table_entry_ptr
    handle_node_data.handle_table_entry_address = handle_table_entry_ptr;

    // grantedaccess
    handle_node_data.grantedaccess = grantedaccess;

    // true_typr
    handle_node_data.type = true_type;

    // detail 
    handle_node_data.detail = detail ? detail : calloc(1, sizeof(char));

    utarray_push_back(handle_node, &handle_node_data);
}



/*******************************************************************
static char* parse_unicode_str_ptr(uint64_t ustr_ptr, CPUState *cpu)

parse unicode string from address "ustr_ptr", this is at begining of structure _UNICODE_STRING

INPUT:     uint64_t ustr_ptr,        the begining address of structure _UNICODE_STRING
           CPUState *cpu,            the pointer to current cpu
OUTPUT:    char*                     return the ascii string
*******************************************************************/
static char* parse_unicode_str_ptr(uint64_t ustr_ptr, CPUState *cpu)
{
    int i;
    uint64_t buf_ptr;
    uint16_t length, max_length;
    uint8_t *buf;
    char* str;

    int offset_maxlength_to_unicode,
        offset_length_to_unicode,
        offset_buffer_to_unicode;


    // Get maximum length
    memfrs_get_nested_field_offset(&offset_maxlength_to_unicode, "_UNICODE_STRING", 1, "MaximumLength");
    cpu_memory_rw_debug( cpu, ustr_ptr+offset_maxlength_to_unicode, (uint8_t*)&max_length, sizeof(max_length), 0 );

    // Get length
    memfrs_get_nested_field_offset(&offset_length_to_unicode, "_UNICODE_STRING", 1, "Length");
    cpu_memory_rw_debug( cpu, ustr_ptr+offset_length_to_unicode, (uint8_t*)&length, sizeof(length), 0 );

    if(length == 0 || length > 256 || max_length ==0 || max_length > 256)
        return NULL;

    // Get buffer
    memfrs_get_nested_field_offset(&offset_buffer_to_unicode, "_UNICODE_STRING", 1, "Buffer");
    cpu_memory_rw_debug( cpu, ustr_ptr+offset_buffer_to_unicode, (uint8_t*)&buf_ptr, sizeof(buf_ptr), 0 );


    buf = (uint8_t*)malloc(max_length+2);
    str = (char*)malloc(max_length+1);
    memset(str, 0, max_length+1);
    cpu_memory_rw_debug( cpu, buf_ptr, buf, max_length, 0 );
    // Hardcode Unicode Parse
    for(i=0; i<max_length;i+=2)
        str[i/2] = buf[i];   
    str[i] = 0x00;

    free(buf);
    return str;
}



/*******************************************************************
static void extract_file_detail( uint64_t handle_table_entry_ptr, CPUState *cpu )

extract handle table name info detail from handle_table_entry_name_info of handle table entry

INPUT:     uint64_t handle_table_entrt_ptr,   the address of handle table entry
           uint64_t cr3,                      the current cr3
           CPUState *cpu,                     the pointer to current cpu
OUTPUT:    char*                              return the string of name info detail
*******************************************************************/
static char* extract_entry_name_info_detail( uint64_t handle_table_entry_ptr, uint64_t cr3, CPUState *cpu )
{
    uint64_t object_header_name_info_ptr;
    uint8_t infomask;
    char *name_info_detail = NULL;

    int offset_name_to_object_header_name_info = 0;

    // Get Name offset from _OBJECT_HEADER_NAME_INFO
    memfrs_get_nested_field_offset(&offset_name_to_object_header_name_info, "_OBJECT_HEADER_NAME_INFO", 1, "Name");

    memfrs_get_mem_struct_content( cpu, cr3, (uint8_t*)&infomask, sizeof(infomask), handle_table_entry_ptr, false, "_OBJECT_HEADER", 1, "#InfoMask");
    if( infomask & 0x2 ){
        if( infomask & 0x1 )
            object_header_name_info_ptr = handle_table_entry_ptr-_OBJECT_HEADER_CREATOR_INFO-_OBJECT_HEADER_NAME_INFO;
        else
            object_header_name_info_ptr = handle_table_entry_ptr-_OBJECT_HEADER_NAME_INFO;

        name_info_detail = parse_unicode_str_ptr(object_header_name_info_ptr+offset_name_to_object_header_name_info, cpu);
    }

    return name_info_detail;
}



/*******************************************************************
static char* extract_process_detail( uint64_t handle_table_entry_ptr, uint64_t cr3, CPUState *cpu )

extract registry detail from handle table entry

INPUT:     uint64_t handle_table_entrt_ptr,   the address of handle table entry
           uint64_t cr3,                      the current cr3
           CPUState *cpu,                     the pointer to current cpu
OUTPUT:    char*                              return the string of process detail
*******************************************************************/
static char* extract_process_detail( uint64_t handle_table_entry_ptr, uint64_t cr3, CPUState *cpu )
{
    uint64_t handle_table_entry_body_ptr;
    uint64_t unique_process_id;
    char *process_detail = NULL;
    uint8_t buf[16]={0};

    handle_table_entry_body_ptr = handle_table_entry_ptr+g_body_to_object_header;
    memfrs_get_mem_struct_content( cpu, cr3, (uint8_t*)buf, sizeof(buf), handle_table_entry_body_ptr, false, "_EPROCESS", 1, "#ImageFileName");
    memfrs_get_mem_struct_content( cpu, cr3, (uint8_t*)&unique_process_id, sizeof(unique_process_id), handle_table_entry_body_ptr, false, "_EPROCESS", 1, "#UniqueProcessId");

    process_detail = (char *)malloc(32);
    memset(process_detail, 0, 32);
    sprintf(process_detail, "{%s}({%"PRIx64"})", buf, unique_process_id);

    return process_detail;
}



/*******************************************************************
static char* extract_registry_detail( uint64_t handle_table_entry_ptr, uint64_t cr3, CPUState *cpu )

extract registry detail from handle table entry

INPUT:     uint64_t handle_table_entrt_ptr,   the address of handle table entry
           uint64_t cr3,                      the current cr3
           CPUState *cpu,                     the pointer to current cpu
OUTPUT:    char*                              return the string of registry detail
*******************************************************************/
static char* extract_registry_detail( uint64_t handle_table_entry_ptr, uint64_t cr3, CPUState *cpu )
{
    int i;
    char *tmp_registry_detail = NULL,
         *registry_detail = NULL;

    uint64_t handle_table_entry_body_ptr, 
             kcb_ptr,
             kcb_parent_ptr;
    uint16_t key_length, total_key_length=0, tmp_key_length=0; 

    // [XXX] registry name
    // +0x01a Name to 0x20 : [1] Wchar
    // Need to check the size
    uint8_t name[256];

    handle_table_entry_body_ptr = handle_table_entry_ptr+g_body_to_object_header;
    memfrs_get_mem_struct_content( cpu, cr3, (uint8_t*)&kcb_ptr, sizeof(kcb_ptr), handle_table_entry_body_ptr, false, "_CM_KEY_BODY", 1, "*KeyControlBlock");

    while( memfrs_get_mem_struct_content( cpu, cr3, (uint8_t*)&kcb_parent_ptr, sizeof(kcb_parent_ptr), kcb_ptr, false, "_CM_KEY_CONTROL_BLOCK", 1, "*ParentKcb") !=-1 ){

        memset(name, 0, 256);
        if( memfrs_get_mem_struct_content( cpu, cr3, (uint8_t*)&name, sizeof(name), kcb_ptr, false, "_CM_KEY_CONTROL_BLOCK", 2, "*NameBlock", "#Name") ==-1 )
            break;

        memfrs_get_mem_struct_content( cpu, cr3, (uint8_t*)&key_length, sizeof(key_length), kcb_ptr, false, "_CM_KEY_CONTROL_BLOCK", 2, "*NameBlock", "#NameLength");

        total_key_length = total_key_length + key_length+1 ;

        if(tmp_registry_detail)
            free(tmp_registry_detail);
        tmp_registry_detail = registry_detail;
        registry_detail = (char*)malloc(total_key_length);

        for( i=0 ; i<key_length ; i=i+1 )
            registry_detail[i] = (char)(*(name+i));
        registry_detail[key_length]='\\';

        if(tmp_registry_detail){
            for( i=0; i< tmp_key_length ; i=i+1 )
                registry_detail[key_length+1+i] = tmp_registry_detail[i];
        }

        tmp_key_length = total_key_length;

        kcb_ptr = kcb_parent_ptr;
    }

    if(total_key_length>0){
        registry_detail[total_key_length-1]='\0';
        return registry_detail;
    }
    else
        return NULL;
}



/*******************************************************************
static char* extract_thread_detail( uint64_t handle_table_entry_ptr, uint64_t cr3, CPUState *cpu )

extract thread detail from handle table entry

INPUT:     uint64_t handle_table_entrt_ptr,   the address of handle table entry
           uint64_t cr3,                      the current cr3
           CPUState *cpu,                     the pointer to current cpu
OUTPUT:    char*                              return the string of thread detail
*******************************************************************/
static char* extract_thread_detail( uint64_t handle_table_entry_ptr, uint64_t cr3, CPUState *cpu )
{
    uint64_t handle_table_entry_body_ptr;
    unsigned int uniqueProcess, uniqueThread;
    char *thread_detail;
    // [XXX] Maybe not greater than 32
    thread_detail = (char*)malloc(32+1);
    memset( thread_detail, 0, 32+1);

    handle_table_entry_body_ptr = handle_table_entry_ptr+g_body_to_object_header;
    if ( memfrs_get_mem_struct_content( cpu, cr3, (uint8_t*)&uniqueProcess, sizeof(uniqueProcess), handle_table_entry_body_ptr, false, "_ETHREAD", 2, "#Cid", "#UniqueProcess") !=-1 )
        if ( memfrs_get_mem_struct_content( cpu, cr3, (uint8_t*)&uniqueThread, sizeof(uniqueThread), handle_table_entry_body_ptr, false, "_ETHREAD", 2, "#Cid", "#UniqueThread") !=-1 )
            sprintf(thread_detail, "{TID %d PID %d}", uniqueProcess, uniqueThread);

    return thread_detail;
}



/*******************************************************************
static char* extract_file_detail( uint64_t handle_table_entry_ptr, uint64_t cr3, CPUState *cpu )

extract file detail from handle table entry

INPUT:     uint64_t handle_table_entrt_ptr,   the address of handle table entry
           uint64_t cr3,                      the current cr3
           CPUState *cpu,                     the pointer to current cpu
OUTPUT:    char*                              return the string of file detail
*******************************************************************/
static char* extract_file_detail( uint64_t handle_table_entry_ptr, uint64_t cr3, CPUState *cpu )
{
    uint64_t handle_table_entry_body_ptr,
             Device_entry_body_ptr;
    uint8_t device_infomask;
    char *device_detail = NULL,
         *file_detail = NULL,
         *device_file_detail = NULL;
    uint16_t max_file_length = 0;

    int offset_filename_to_file_object = 0;

    // Get FileName offset from _FILE_OBJECT
    memfrs_get_nested_field_offset(&offset_filename_to_file_object, "_FILE_OBJECT", 1, "FileName");


    // Check and extract device path info
    handle_table_entry_body_ptr = handle_table_entry_ptr+g_body_to_object_header;
    memfrs_get_mem_struct_content( cpu, cr3, (uint8_t*)&Device_entry_body_ptr, sizeof(Device_entry_body_ptr), handle_table_entry_body_ptr, false, "_DEVICE_OBJECT", 1, "*DriverObject");
    if( memfrs_get_mem_struct_content( cpu, cr3, (uint8_t*)&device_infomask, sizeof(device_infomask), Device_entry_body_ptr-g_body_to_object_header, false, "_OBJECT_HEADER", 1, "#InfoMask") !=-1 )
        device_detail = extract_entry_name_info_detail( Device_entry_body_ptr-g_body_to_object_header, cr3, cpu );

    file_detail = parse_unicode_str_ptr(handle_table_entry_body_ptr+offset_filename_to_file_object, cpu);

    if(device_detail!=NULL){

        if(file_detail!=NULL){
            max_file_length = strlen(device_detail)+strlen(file_detail);
            // "\Device\" length is 8, and end of string is 1
            device_file_detail = (char*)malloc(8+max_file_length+1);
            memset(device_file_detail, 0, 8+max_file_length+1);
            sprintf(device_file_detail, "\\Device\\%s%s", device_detail, file_detail);
        }
        else{
            max_file_length = strlen(device_detail);
            // "\Device\" length is 8, and end of string is 1
            device_file_detail = (char*)malloc(8+max_file_length+1);
            memset(device_file_detail, 0, 8+max_file_length+1);
            sprintf(device_file_detail, "\\Device\\%s", device_detail);
        }

        return device_file_detail;

    }
    else if(file_detail!=NULL)
        return file_detail;

    return NULL;
}



/*******************************************************************
static void do_table_entry( int entry_index, uint64_t handle_table_entry_ptr, uint64_t grantedaccess, uint64_t cr3, UT_array *process, CPUState *cpu )

extract data of handle

INPUT:     int entry_index                    the index of level 0
           uint64_t handle_table_entrt_ptr,   the address of handle table entry
           uint64_t grantedaccess             the granted access of handle
           uint64_t cr3,                      the current cr3
           UT_array *process,                 the UT_array structure with handles data of one process
           CPUState *cpu,                     the pointer to current cpu
OUTPUT:    void
*******************************************************************/
static void do_table_entry( int entry_index, uint64_t handle_table_entry_ptr, uint64_t grantedaccess, uint64_t cr3, UT_array *process, CPUState *cpu )
{
    uint64_t object_type_ptr;
    uint8_t cookie,
            type_index,
            true_type_index;
    char *true_type_name;
    char *detail = NULL;


    uint64_t nt_kernel_base = 0,
             g_ObTypeIndexTable_ptr,
             g_ObHeaderCookie_ptr;
    json_object *gvar = NULL;
    int offset_name_to_object_type = 0;

    // Get Name offset from _OBJECT_TYPE
    memfrs_get_nested_field_offset(&offset_name_to_object_type, "_OBJECT_TYPE", 1, "Name"); 


    // Get global variable "ObTypeIndexTable" address 
    nt_kernel_base = memfrs_get_nt_kernel_base();
    if(nt_kernel_base == 0)
        nt_kernel_base = memfrs_find_nt_kernel_base(cpu);
    gvar = memfrs_q_globalvar("ObTypeIndexTable");
    g_ObTypeIndexTable_ptr = memfrs_gvar_offset(gvar) + nt_kernel_base;

    // # In Windows 10 the type index is obfuscated
    // Windows 10 obfuscates the object type using a cookie:
    // 
    // ------ nt!ObpRemoveObjectRoutine ------: reversing
    // Cookie stored in global variable ObHeaderCookie
    //
    // Get global variable "ObHeaderCookie" value
    gvar = memfrs_q_globalvar("ObHeaderCookie");
    g_ObHeaderCookie_ptr = memfrs_gvar_offset(gvar) + nt_kernel_base;
    cpu_memory_rw_debug( cpu, g_ObHeaderCookie_ptr, (uint8_t*)&cookie, sizeof(cookie), 0 );


    // Handle table entry format in Windows10
    // Unlocked        : Bitfield Pos 0, 1 Bit
    // RefCnt          : Bitfield Pos 1, 16 Bit
    // Attributes      : Bitfield Pos 17, 3 Bit
    // ObjectPointerBits : Bitfield Pos 20, 44 Bit
    // GrantedAccessBits : Bitfield Pos 0, 25 Bit
    // NoRightsUpgrade : Bitfield Pos 25, 1 Bit
    // Spare1          : Bitfield Pos 26, 6 Bit
    // Spare2          : Uint4B
    if( memfrs_get_mem_struct_content( cpu, cr3, (uint8_t*)&type_index, sizeof(type_index), handle_table_entry_ptr, false, "_OBJECT_HEADER", 1, "#TypeIndex") !=-1 ){
        true_type_index =  (uint8_t)((type_index ^ cookie ^ ((handle_table_entry_ptr & 0x0000ffffffffffff)>>8))& 0xff );

        cpu_memory_rw_debug( cpu, g_ObTypeIndexTable_ptr + 0x8*true_type_index , (uint8_t*)&object_type_ptr, sizeof(object_type_ptr), 0 );
        true_type_name = parse_unicode_str_ptr(object_type_ptr+offset_name_to_object_type, cpu);

        if( strcmp(true_type_name, "File")==0 )
            detail = extract_file_detail(handle_table_entry_ptr, cr3, cpu);

        else if( strcmp(true_type_name, "Thread")==0 )
            detail = extract_thread_detail(handle_table_entry_ptr, cr3, cpu);

        else if( strcmp(true_type_name, "Key")==0 )
            detail = extract_registry_detail(handle_table_entry_ptr, cr3, cpu);

        else if( strcmp(true_type_name, "Process")==0 )
            detail = extract_process_detail(handle_table_entry_ptr, cr3, cpu);

        else
            detail = extract_entry_name_info_detail(handle_table_entry_ptr, cr3, cpu);


        add_handle_field_to_struct( process, entry_index*4, handle_table_entry_ptr, grantedaccess, true_type_name, detail );
    }
}



/*******************************************************************
static int entry_is_legal( uint64_t handle_table_entry_ptr, uint64_t cr3, CPUState *cpu )

Check whether handle table entry is legal

INPUT:     uint64_t handle_table_entrt_ptr,   the address of handle table entry
           uint64_t cr3,                      the current cr3
           CPUState *cpu,                     the pointer to current cpu
OUTPUT:    int                                return 1 if entry is legal
*******************************************************************/
static int entry_is_legal( uint64_t handle_table_entry_ptr, uint64_t cr3, CPUState *cpu )
{
    int pointercount, handlecount;
    uint8_t typeindex;
    uint8_t infomask;

    if( memfrs_get_mem_struct_content( cpu, cr3, (uint8_t*)&pointercount, sizeof(pointercount), handle_table_entry_ptr, false, "_OBJECT_HEADER", 1, "#PointerCount") !=-1 )
        if( pointercount > 0x1000000 || pointercount < 0 )
            return 0;

    if( memfrs_get_mem_struct_content( cpu, cr3, (uint8_t*)&handlecount, sizeof(handlecount), handle_table_entry_ptr, false, "_OBJECT_HEADER", 1, "#HandleCount") !=-1 )
        if( handlecount > 0x1000 || handlecount < 0 )
            return 0;

    // # This specifies the order the headers are found below the _OBJECT_HEADER
    // optional_header_mask = (
    //     ('CreatorInfo', '_OBJECT_HEADER_CREATOR_INFO', 0x01),
    //     ('NameInfo',    '_OBJECT_HEADER_NAME_INFO',    0x02),
    //     ('HandleInfo',  '_OBJECT_HEADER_HANDLE_INFO',  0x04),
    //     ('QuotaInfo',   '_OBJECT_HEADER_QUOTA_INFO',   0x08),
    //     ('ProcessInfo', '_OBJECT_HEADER_PROCESS_INFO', 0x10),
    //     ('AuditInfo',   '_OBJECT_HEADER_AUDIT_INFO',   0x20),
    //     ('PaddingInfo', '_OBJECT_HEADER_PADDING_INFO', 0x40)
    // )
    if( memfrs_get_mem_struct_content( cpu, cr3, (uint8_t*)&infomask, sizeof(infomask), handle_table_entry_ptr, false, "_OBJECT_HEADER", 1, "#InfoMask") !=-1 )
        if( infomask > 0x7f )
            return 0;

    // the range of type index is 2 to about 53 ,
    // lower bound is 2, but higher bound depending on Windows version
    if( memfrs_get_mem_struct_content( cpu, cr3, (uint8_t*)&typeindex, sizeof(typeindex), handle_table_entry_ptr, false, "_OBJECT_HEADER", 1, "#TypeIndex") !=-1 )
        if( typeindex >53 || typeindex < 1 )
            return 0;

    return 1;
}



/*******************************************************************
static void do_table_level_0( int level1_index, uint64_t level0_ptr, uint64_t cr3, UT_array *process, CPUState *cpu )

Find handle table entry in level 0 from handle table

INPUT:     int level1_index          the index of level 1
           uint64_t level0_ptr,      the address of level 0
           uint64_t cr3,             the current cr3
           UT_array *process,        the UT_array structure with handles data of one process
           CPUState *cpu,            the pointer to current cpu
OUTPUT:    void
*******************************************************************/
static void do_table_level_0( int level1_index, uint64_t level0_ptr, uint64_t cr3, UT_array *process, CPUState *cpu )
{
    int i;
    uint64_t handle_table_entry_ptr,
             grantedaccess;

    // Every 0x10 stored a handle table entry address in level 0 of handle table
    for( i=1 ; i<=256 ; i=i+1 ){
        level0_ptr = level0_ptr+0x10;
        cpu_memory_rw_debug( cpu, level0_ptr, (uint8_t*)&handle_table_entry_ptr, sizeof(handle_table_entry_ptr), 0 );
        handle_table_entry_ptr = ( ( (handle_table_entry_ptr & 0xfffffffffff00000) >> 16 ) + 0xffff000000000000);

        // Check whether handle table entry is legal
        if( entry_is_legal(handle_table_entry_ptr, cr3, cpu) ==0 )
            continue;

        // [XXX] meaning of granted access still unknow
        cpu_memory_rw_debug( cpu, level0_ptr+0x8, (uint8_t*)&grantedaccess, sizeof(grantedaccess), 0 );
        grantedaccess = grantedaccess & 0x0000000001ffffff;

        do_table_entry( i+level1_index*256, handle_table_entry_ptr, grantedaccess, cr3, process, cpu );
    }
}



/*******************************************************************
static void do_table_level_1( int level2_index, uint64_t level1_ptr, uint64_t cr3, UT_array *process, CPUState *cpu )

Find level 0 in level 1 from handle table

INPUT:     int level2_index,         the index of level 2 table
           uint64_t level1_ptr,      the address of level 1
           uint64_t cr3,             the current cr3
           UT_array *process,        the UT_array structure with handles data of one process
           CPUState *cpu,            the pointer to current cpu
OUTPUT:    void
*******************************************************************/
static void do_table_level_1( int level2_index, uint64_t level1_ptr, uint64_t cr3, UT_array *process, CPUState *cpu )
{
    int i=0;
    uint64_t level0_ptr;

    // Every 0x8 stored a level 0 address in level 1 of handle table
    while( i<512 && cpu_memory_rw_debug( cpu, level1_ptr, (uint8_t*)&level0_ptr, sizeof(level0_ptr), 0) !=-1 ){
        if(level0_ptr==0x0)
            break;
        do_table_level_0( i+level2_index*256, level0_ptr, cr3, process, cpu );
        i = i+1;
        level1_ptr = level1_ptr+0x8;
    }
}



/*******************************************************************
static void do_table_level_2( uint64_t level2_ptr, uint64_t cr3, UT_array *process, CPUState *cpu )

Find level 1 in level 2 from handle table

INPUT:     uint64_t level2_ptr,      the address of level 2 table
           uint64_t cr3,             the current cr3
           UT_array *process,        the UT_array structure with handles data of one process
           CPUState *cpu,            the pointer to current cpu
OUTPUT:    void
*******************************************************************/
static void do_table_level_2( uint64_t level2_ptr, uint64_t cr3, UT_array *process, CPUState *cpu )
{

    /* [XXX] Not yet find level2 sample */

    int i=0;
    uint64_t level1_ptr;
    while( cpu_memory_rw_debug( cpu, level2_ptr, (uint8_t*)&level1_ptr, sizeof(level1_ptr), 0) !=-1 ){
        if(level1_ptr==0x0)
            break;
        do_table_level_0( i, level1_ptr, cr3, process, cpu );
        i = i+1;        
        level2_ptr = level2_ptr+0x8;
    }        

}



static void handles_node_dtor(void *_elt) {
    handles_node_st *elt = (handles_node_st*)_elt;
    if(elt->type) free(elt->type);
    if(elt->detail) free(elt->detail);
}

static void handles_dtor(void *_elt) {
    handles_st *elt = (handles_st*)_elt;

    if(elt->imagename) free(elt->imagename);
    if(elt->handles_node) utarray_free(elt->handles_node);
}

UT_icd hanldes_node_icd = {sizeof(handles_node_st), NULL, NULL, handles_node_dtor};
UT_icd hanldes_icd = {sizeof(handles_st), NULL, NULL, handles_dtor};
/*******************************************************************
extern UT_array* memfrs_enum_proc_handles( int target_type, uint64_t target, uint64_t kpcr_ptr, CPUState *cpu )

Eumerate the running process handles

INPUT:     int target_type,          searching type of handles
           uint64_t target,          searching target
           uint64_t kpcr_ptr,        the address of _KPCR struct
           CPUState *cpu,            the pointer to current cpu
OUTPUT:    UT_array*                 return a UT_array with handles data
*******************************************************************/
extern UT_array* memfrs_enum_proc_handles( int target_type, uint64_t target, uint64_t kpcr_ptr, CPUState *cpu )
{
    // final return
    UT_array *process_handles,
             *process_handles_node;
    handles_st handles;

    int i;
    int count =0;
    uint64_t cr3,
             kthread_ptr,
             eprocess_ptr_init = 0,
             eprocess_ptr,
             tablecode_ptr;
    uint64_t processid;

    uint8_t buf[15];

    int offset_entry_list_to_eprocess = 0;

    int table_level;


    // Check if the data structure information is loaded
    if(g_struct_info == NULL)
    {
        memfrs_errno = MEMFRS_ERR_NOT_LOADED_STRUCTURE;
        return NULL;
    }
    // Check if the global data structure information is loaded
    if(g_globalvar_info == NULL)
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

    // Read the concrete memory value of kthread_ptr(CurrentThread) via _KPCR address
    memfrs_get_mem_struct_content( cpu, 0, (uint8_t*)&kthread_ptr, sizeof(kthread_ptr), kpcr_ptr, false, "_KPCR", 2, "#Prcb", "#CurrentThread");

    // Read the concrete memory value of PROCESS via CurrentThread
    // Get the first PROCESS
    memfrs_get_mem_struct_content( cpu, 0, (uint8_t*)&eprocess_ptr, sizeof(eprocess_ptr), kthread_ptr, false, "_KTHREAD", 1, "#Process");

    // Get ActiveProcessLinks offset from _EPROCESS
    memfrs_get_nested_field_offset(&offset_entry_list_to_eprocess, "_EPROCESS", 1, "ActiveProcessLinks");

    // Get Body offset from _OBJECT_HEADER
    memfrs_get_nested_field_offset(&g_body_to_object_header, "_OBJECT_HEADER", 1, "Body");


    // Assign process_handles be a 'handles_st' structure UTarray
    utarray_new( process_handles, &hanldes_icd);


    // Start iteration process list
    eprocess_ptr_init = eprocess_ptr;

    do {
        //Read CR3 & Process name
        memfrs_get_mem_struct_content( cpu, 0, (uint8_t*)&cr3, sizeof(cr3), eprocess_ptr, false, "_EPROCESS", 2, "#Pcb", "#DirectoryTableBase");
        memfrs_get_mem_struct_content( cpu, 0, (uint8_t*)buf, sizeof(buf), eprocess_ptr, false, "_EPROCESS", 1, "#ImageFileName");
        memfrs_get_mem_struct_content( cpu, 0, (uint8_t*)&processid, sizeof(processid), eprocess_ptr, false, "_EPROCESS", 1, "#UniqueProcessId");

        if( cr3!=0 ){

            utarray_new( process_handles_node, &hanldes_node_icd );

            if( target_type==PARSING_HANDLE_TYPE_ALL || 
                    ( target_type==PARSING_HANDLE_TYPE_CR3 && target==cr3 ) || 
                    ( target_type==PARSING_HANDLE_TYPE_EPROCESS && target==eprocess_ptr ) || 
                    ( target_type==PARSING_HANDLE_TYPE_PID && target==processid ) ){
                memfrs_get_mem_struct_content( cpu, cr3, (uint8_t*)&tablecode_ptr, sizeof(tablecode_ptr), eprocess_ptr, false, "_EPROCESS", 2, "*ObjectTable", "#TableCode");
                table_level = tablecode_ptr & 0x3;
                tablecode_ptr = tablecode_ptr & 0xfffffffffffffffc ;

                // table level == 0 means we are at the bottom level and this is a table of _HANDLE_TABLE_ENTRY
                // otherwise, it means we are a table of pointers to lower tables.
                if( table_level == 0 )
                    do_table_level_0(0, tablecode_ptr, cr3, process_handles_node, cpu);

                else if ( table_level == 1 )
                    do_table_level_1(0, tablecode_ptr, cr3, process_handles_node, cpu);

                else 
                    do_table_level_2(tablecode_ptr, cr3, process_handles_node, cpu);

                handles.CR3 = cr3;
                handles.eprocess = eprocess_ptr;
                handles.pid = processid;
                handles.imagename = (char*)malloc(15);
                for(i=0;i<15;i=i+1)
                    handles.imagename[i]=buf[i];
                handles.handles_node = process_handles_node;
                utarray_push_back(process_handles, &handles);

            }
        }

        count++;
        if(count > 1000)
            break;

        // Read next entry
        memfrs_get_mem_struct_content( cpu, 0, (uint8_t*)&eprocess_ptr, sizeof(eprocess_ptr), eprocess_ptr, false, "_EPROCESS", 2, "#ActiveProcessLinks", "*Blink");
        // Substract entry_list offset to find base address of eprocess
        eprocess_ptr = eprocess_ptr-offset_entry_list_to_eprocess;
    }while( eprocess_ptr != eprocess_ptr_init );

    return process_handles;
}



/*******************************************************************
extern UT_array* memfrs_enum_proc_handles_detail( int target_type, const char* target, uint64_t kpcr_ptr, CPUState *cpu )

Eumerate the running process handles, expect for types and details

INPUT:     int target_type,          searching type of handles
           const char* target        searching target
           uint64_t kpcr_ptr,        the address of _KPCR struct
           CPUState *cpu,            the pointer to current cpu
OUTPUT:    UT_array*                 return a UT_array with handles data
*******************************************************************/
extern UT_array* memfrs_enum_proc_handles_detail( int target_type, const char* target, uint64_t kpcr_ptr, CPUState *cpu )
{
    // final return
    UT_array *process_handles,
             *process_handles_node;
    handles_st handles;

    //tmp handles
    UT_array *tmp_process_handles;
    handles_st *tmp_handles;
    handles_node_st *tmp_handles_node;

    int has_entry;

    tmp_process_handles = memfrs_enum_proc_handles( PARSING_HANDLE_TYPE_ALL, 0, kpcr_ptr, cpu );
    if( tmp_process_handles == NULL)
        return NULL;


    // Assign process_handles be a 'handles_st' structure UTarray
    utarray_new( process_handles, &hanldes_icd);


    tmp_handles = NULL;
    while( (tmp_handles=(handles_st*)utarray_next(tmp_process_handles,tmp_handles)) ){

        tmp_handles_node = NULL;
        has_entry = 0;
        utarray_new( process_handles_node, &hanldes_node_icd);

        while( (tmp_handles_node=(handles_node_st*)utarray_next(tmp_handles->handles_node, tmp_handles_node)) ){
            if( ( target_type==PARSING_HANDLE_TYPE_TYPE && strcmp(tmp_handles_node->type, target)==0 ) || 
                    ( target_type==PARSING_HANDLE_TYPE_FULL_DETAIL && strcmp(tmp_handles_node->detail, target)==0 ) || 
                    ( target_type==PARSING_HANDLE_TYPE_DETAIL && strstr(tmp_handles_node->detail, target) ) ){
                has_entry=1;
                // add_handle_field_to_struct(UT_array *handle_node, int entry_index, uint64_t handle_table_entry_ptr, uint64_t grantedaccess, char* true_type, char* detail)
                add_handle_field_to_struct( process_handles_node, 
                        tmp_handles_node->handle_table_entry_index , 
                        tmp_handles_node->handle_table_entry_address, 
                        tmp_handles_node->grantedaccess,
                        tmp_handles_node->type, 
                        tmp_handles_node->detail );
            }
        }

        if( has_entry ){
            handles.CR3 = tmp_handles->CR3;
            handles.eprocess = tmp_handles->eprocess;
            handles.pid = tmp_handles->pid;
            handles.imagename = (char*)malloc(15);
            sprintf(handles.imagename, "%s", tmp_handles->imagename);
            handles.handles_node = process_handles_node;
            utarray_push_back(process_handles, &handles);
        }
        else
            free(process_handles_node);
    }

    free(tmp_process_handles);
    return process_handles;
}



/*******************************************************************
extern UT_array* memfrs_enum_handles_types( uint64_t kpcr_ptr, CPUState *cpu )

Eumerate the handles types

INPUT:     uint64_t kpcr_ptr,        the address of _KPCR struct
           CPUState *cpu,            the pointer to current cpu
OUTPUT:    UT_array*                 return a UT_array with handles types
*******************************************************************/
extern UT_array* memfrs_enum_handles_types( uint64_t kpcr_ptr, CPUState *cpu )
{
    int i;
    UT_array *handles_types = NULL;

    uint64_t object_type_ptr,
             type_buf_ptr;
    int type_index;
    char *type_name = NULL;
    char type_buf[256];
    uint16_t length;

    uint64_t nt_kernel_base = 0,
             g_ObTypeIndexTable_ptr;
    json_object *gvar = NULL;
    int offset_name_to_object_type = 0;

    // Check if the data structure information is loaded
    if(g_struct_info == NULL)
    {
        memfrs_errno = MEMFRS_ERR_NOT_LOADED_STRUCTURE;
        return NULL;
    }
    // Check if the global data structure information is loaded
    if(g_globalvar_info == NULL)
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

    // Get Name offset from _OBJECT_TYPE
    if( memfrs_get_nested_field_offset(&offset_name_to_object_type, "_OBJECT_TYPE", 1, "Name") ==-1 )
        return NULL;

    // Get global variable "ObTypeIndexTable" address 
    nt_kernel_base = memfrs_get_nt_kernel_base();
    if(nt_kernel_base == 0)
        nt_kernel_base = memfrs_find_nt_kernel_base(cpu);
    gvar = memfrs_q_globalvar("ObTypeIndexTable");
    g_ObTypeIndexTable_ptr = memfrs_gvar_offset(gvar) + nt_kernel_base;


    utarray_new( handles_types, &ut_str_icd);


    type_index=0;
    while(type_index<256){
        if ( cpu_memory_rw_debug( cpu, g_ObTypeIndexTable_ptr + 0x8*type_index , (uint8_t*)&object_type_ptr, sizeof(object_type_ptr), 0 ) ==0 ){
            if( memfrs_get_mem_struct_content(cpu, 0, (uint8_t*)&length, sizeof(length), object_type_ptr, false, "_OBJECT_TYPE", 2, "#Name", "#Length") == -1 )
                length=0;
            if( memfrs_get_mem_struct_content(cpu, 0, (uint8_t*)&type_buf_ptr, sizeof(type_buf_ptr), object_type_ptr, false, "_OBJECT_TYPE", 2, "#Name", "*Buffer") == -1 )
                type_buf_ptr=0;

            if( type_buf_ptr!=0 && memfrs_get_virmem_content( cpu, 0, type_buf_ptr, sizeof(type_buf), (uint8_t*)type_buf ) != -1 ){

                type_name=(char*)malloc(length/2+1);
                for(i=0;i<length;i=i+1){
                    if( i%2==0 )
                        type_name[i/2]=(char)(*(type_buf+i));
                }
                type_name[length/2]='\0';

                utarray_push_back(handles_types, &type_name);
            }
        }
        type_index = type_index+1;
    }

    return handles_types;
}
