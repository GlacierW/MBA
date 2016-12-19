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

#if !defined(CONFIG_MEMFRS_TEST)
#include "qemu-common.h"
#include "monitor/monitor.h"
#include "include/exec/cpu-common.h"
#include "exec/cpu-all.h"
#include "include/utarray.h"
#include "include/uthash.h"
#include "json-c/json.h"
#endif

#include "memfrs.h"
#include "memfrs-priv.h"
#include<stdlib.h>

#define _OBJECT_HEADER_CREATOR_INFO 0x20
#define _OBJECT_HEADER_NAME_INFO 0x20
#define _OBJECT_HEADER_HANDLE_INFO 0x10
#define _OBJECT_HEADER_QUOTA_INFO 0x20
#define _OBJECT_HEADER_PROCESS_INFO 0x10
#define _OBJECT_HEADER_AUDIT_INFO 0x10
#define _OBJECT_HEADER_PADDING_INFO 0x4

static int g_body_to_object_header;



/*******************************************************************
static void add_handle_field_to_struct(json_object *parent_obj, int entry_index, uint64_t handle_table_entry_ptr, uint64_t grantedaccess, char* true_type, char* detail)

add a handle entry data in json structure process

INPUT:     json_object *parent_obj,             target json object
           int entry_index,                     entry table index
           uint64_t handle_table_entry_ptr,     address of handle table entey 
           uint64_t grantedaccess,              granted access
           char* true_type,                     true type of handle table entry
           char* detail                         details of handles table entry
OUTPUT:    void
*******************************************************************/
static void add_handle_field_to_struct(json_object *parent_obj, int entry_index, uint64_t handle_table_entry_ptr, uint64_t grantedaccess, char* true_type, char* detail)
{
    json_object *field_info;
    field_info = json_object_new_array();
    char str_handle_table_entry[19]={0};
    char str_grantedaccess[19]={0};
    char str_entry_index[8] = {0};

    // handle_table_entry_ptr
    sprintf(str_handle_table_entry, "0x%"PRIx64, handle_table_entry_ptr);
    json_object_array_add(field_info, json_object_new_string(str_handle_table_entry));

    // grantedaccess
    sprintf(str_grantedaccess, "0x%"PRIx64, grantedaccess);
    json_object_array_add(field_info, json_object_new_string(str_grantedaccess));

    // true_typr
    json_object_array_add(field_info, json_object_new_string(true_type));

    // detail 
    if(detail)
        json_object_array_add(field_info, json_object_new_string(detail));
    else
        json_object_array_add(field_info, json_object_new_string(""));

    // entry_index
    sprintf(str_entry_index, "0x%"PRIx32, entry_index);
    json_object_object_add(parent_obj, str_entry_index, field_info);

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
    int offset;
    json_object* ustr = NULL;
    field_info* f_info = NULL;
    uint16_t length, max_length;
    uint64_t buf_ptr;
    uint8_t *buf;
    char* str;

    ustr = memfrs_q_struct("_UNICODE_STRING");

    // Get maximum length
    f_info = memfrs_q_field(ustr, "MaximumLength");
    offset = f_info->offset;
    cpu_memory_rw_debug( cpu, ustr_ptr+offset, (uint8_t*)&max_length, sizeof(max_length), 0 );
    memfrs_close_field(f_info);

    // Get length
    f_info = memfrs_q_field(ustr, "Length");
    offset = f_info->offset;
    cpu_memory_rw_debug( cpu, ustr_ptr+offset, (uint8_t*)&length, sizeof(length), 0 );
    memfrs_close_field(f_info);

    if(length == 0 || length > 256 || max_length ==0 || max_length > 256)
        return NULL;

    // Get buffer
    f_info = memfrs_q_field(ustr, "Buffer");
    offset = f_info->offset;
    cpu_memory_rw_debug( cpu, ustr_ptr+offset, (uint8_t*)&buf_ptr, sizeof(buf_ptr), 0 );
    memfrs_close_field(f_info); 

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

extract handle table name info detail from handle table entry name info of handle table entry

INPUT:     handle_table_entrt_ptr,   the address of handle table entry
           uint64_t cr3,             the current cr3
           CPUState *cpu,            the pointer to current cpu
OUTPUT:    char*                     return the string of name info detail
*******************************************************************/
static char* extract_entry_name_info_detail( uint64_t handle_table_entry_ptr, uint64_t cr3, CPUState *cpu )
{
    uint64_t object_header_name_info_ptr;
    uint8_t infomask;
    char *name_info_detail = NULL;

    json_object* jobject_type = NULL;
    int offset_name_to_object_header_name_info = 0;
    field_info* f_info = NULL;

    // Retrieve the _OBJECT_HEADER_NAME_INFO structure
    jobject_type = memfrs_q_struct("_OBJECT_HEADER_NAME_INFO");
    // Query Name in _OBJECT_HEADER_NAME_INFO
    f_info = memfrs_q_field(jobject_type, "Name");
    // Calculate Entry List offset from _OBJECT_HEADER_NAME_INFO
    offset_name_to_object_header_name_info = f_info->offset;
    memfrs_close_field(f_info);


    memfrs_get_virmem_struct_content( cpu, cr3, (uint8_t*)&infomask, sizeof(infomask), handle_table_entry_ptr, "_OBJECT_HEADER", 1, "#InfoMask");
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

INPUT:     handle_table_entrt_ptr,   the address of handle table entry
           uint64_t cr3,             the current cr3
           CPUState *cpu,            the pointer to current cpu
OUTPUT:    char*                     return the string of process detail
*******************************************************************/
static char* extract_process_detail( uint64_t handle_table_entry_ptr, uint64_t cr3, CPUState *cpu )
{
    uint64_t handle_table_entry_body_ptr;
    uint64_t unique_process_id;
    char *process_detail = NULL;
    uint8_t buf[16]={0};

    handle_table_entry_body_ptr = handle_table_entry_ptr+g_body_to_object_header;
    memfrs_get_virmem_struct_content( cpu, cr3, (uint8_t*)buf, sizeof(buf), handle_table_entry_body_ptr, "_EPROCESS", 1, "#ImageFileName");
    memfrs_get_virmem_struct_content( cpu, cr3, (uint8_t*)&unique_process_id, sizeof(unique_process_id), handle_table_entry_body_ptr, "_EPROCESS", 1, "#UniqueProcessId");

    process_detail = (char *)malloc(32);
    memset(process_detail, 0, 32);
    sprintf(process_detail, "{%s}({%"PRIx64"})", buf, unique_process_id);

    return process_detail;
}



/*******************************************************************
static char* extract_registry_detail( uint64_t handle_table_entry_ptr, uint64_t cr3, CPUState *cpu )

extract registry detail from handle table entry

INPUT:     handle_table_entrt_ptr,   the address of handle table entry
           uint64_t cr3,             the current cr3
           CPUState *cpu,            the pointer to current cpu
OUTPUT:    char*                     return the string of registry detail
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
    memfrs_get_virmem_struct_content( cpu, cr3, (uint8_t*)&kcb_ptr, sizeof(kcb_ptr), handle_table_entry_body_ptr, "_CM_KEY_BODY", 1, "*KeyControlBlock");

    while( memfrs_get_virmem_struct_content( cpu, cr3, (uint8_t*)&kcb_parent_ptr, sizeof(kcb_parent_ptr), kcb_ptr, "_CM_KEY_CONTROL_BLOCK", 1, "*ParentKcb") !=-1 ){

        memset(name, 0, 256);
        if( memfrs_get_virmem_struct_content( cpu, cr3, (uint8_t*)&name, sizeof(name), kcb_ptr, "_CM_KEY_CONTROL_BLOCK", 2, "*NameBlock", "#Name") ==-1 )
            break;

        memfrs_get_virmem_struct_content( cpu, cr3, (uint8_t*)&key_length, sizeof(key_length), kcb_ptr, "_CM_KEY_CONTROL_BLOCK", 2, "*NameBlock", "#NameLength");

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

    registry_detail[total_key_length-1]='\0';

    return registry_detail;
}



/*******************************************************************
static char* extract_thread_detail( uint64_t handle_table_entry_ptr, uint64_t cr3, CPUState *cpu )

extract thread detail from handle table entry

INPUT:     handle_table_entrt_ptr,   the address of handle table entry
           uint64_t cr3,             the current cr3
           CPUState *cpu,            the pointer to current cpu
OUTPUT:    char*                     return the string of thread detail
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
    if ( memfrs_get_virmem_struct_content( cpu, cr3, (uint8_t*)&uniqueProcess, sizeof(uniqueProcess), handle_table_entry_body_ptr, "_ETHREAD", 2, "#Cid", "#UniqueProcess") !=-1 )
        if ( memfrs_get_virmem_struct_content( cpu, cr3, (uint8_t*)&uniqueThread, sizeof(uniqueThread), handle_table_entry_body_ptr, "_ETHREAD", 2, "#Cid", "#UniqueThread") !=-1 )
            sprintf(thread_detail, "{TID %d PID %d}", uniqueProcess, uniqueThread);

    return thread_detail;
}



/*******************************************************************
static void extract_file_detail( uint64_t handle_table_entry_ptr, CPUState *cpu )

extract file detail from handle table entry

INPUT:     handle_table_entrt_ptr,   the address of handle table entry
           uint64_t cr3,             the current cr3
           CPUState *cpu,            the pointer to current cpu
OUTPUT:    char*                     return the string of file detail
*******************************************************************/
static char* extract_file_detail( uint64_t handle_table_entry_ptr, uint64_t cr3, CPUState *cpu )
{
    uint64_t handle_table_entry_body_ptr,
             Device_entry_body_ptr;
    uint8_t device_infomask;
    char *device_detail = NULL;
    char *file_detail = NULL;
    char *device_file_detail = NULL;
    uint16_t max_file_length = 0;

    json_object* jobject_type = NULL;
    int offset_filename_to_file_object = 0;
    field_info* f_info = NULL;

    // Retrieve the _FILE_OBJECT structure
    jobject_type = memfrs_q_struct("_FILE_OBJECT");
    // Query FileName in _FILE_OBJECT
    f_info = memfrs_q_field(jobject_type, "FileName");
    // Calculate Entry List offset from _FILE_OBJECT
    offset_filename_to_file_object = f_info->offset;
    memfrs_close_field(f_info);


    // Check and extract device path info
    handle_table_entry_body_ptr = handle_table_entry_ptr+g_body_to_object_header;
    memfrs_get_virmem_struct_content( cpu, cr3, (uint8_t*)&Device_entry_body_ptr, sizeof(Device_entry_body_ptr), handle_table_entry_body_ptr, "_DEVICE_OBJECT", 1, "*DriverObject");
    if( memfrs_get_virmem_struct_content( cpu, cr3, (uint8_t*)&device_infomask, sizeof(device_infomask), Device_entry_body_ptr-g_body_to_object_header, "_OBJECT_HEADER", 1, "#InfoMask") !=-1 )
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
static void do_table_entry( uint64_t grantedaccess, int table_entry_index, int entry_index, uint64_t handle_table_entry_ptr, CPUState *cpu )

extract data of handle

INPUT:     grantedaccess             the granted access of handle
           entry_index               the index of level 0
           handle_table_entrt_ptr,   the address of handle table entry
           uint64_t cr3,             the current cr3
           json_object *process,     the json structure of one process for json structure handles
           CPUState *cpu,            the pointer to current cpu
OUTPUT:    void
*******************************************************************/
static void do_table_entry( uint64_t grantedaccess, int entry_index, uint64_t handle_table_entry_ptr, uint64_t cr3, json_object *process, CPUState *cpu )
{
    uint8_t type_index, true_type_index;

    uint64_t object_type_ptr;

    uint64_t nt_kernel_base = 0,
             g_ObTypeIndexTable_ptr,
             g_ObHeaderCookie_ptr;
    json_object *gvar = NULL;
    uint8_t cookie;
    char *true_type_name;
    char *detail = NULL;

    json_object* jobject_type = NULL;
    int offset_name_to_object_type = 0;
    field_info* f_info = NULL;

    // Retrieve the _OBJECT_TYPE structure
    jobject_type = memfrs_q_struct("_OBJECT_TYPE");
    // Query Name in _OBJECT_TYPE
    f_info = memfrs_q_field(jobject_type, "Name");
    // Calculate Entry List offset from _OBJECT_TYPE
    offset_name_to_object_type = f_info->offset;
    memfrs_close_field(f_info);

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


    if( memfrs_get_virmem_struct_content( cpu, cr3, (uint8_t*)&type_index, sizeof(type_index), handle_table_entry_ptr, "_OBJECT_HEADER", 1, "#TypeIndex") !=-1 ){
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


        add_handle_field_to_struct(process, entry_index*4, handle_table_entry_ptr, grantedaccess, true_type_name, detail);
    }
}



/*******************************************************************
static int entry_is_legal( uint64_t handle_table_entry_ptr, CPUState *cpu )

Check whether handle table entry is legal

INPUT:     handle_table_entrt_ptr,   the address of handle table entry
           uint64_t cr3,             the current cr3
           CPUState *cpu,            the pointer to current cpu
OUTPUT:    int                       return 1 if entry is legal
*******************************************************************/
static int entry_is_legal( uint64_t handle_table_entry_ptr, uint64_t cr3, CPUState *cpu )
{
    int pointercount, handlecount;
    uint8_t typeindex;
    uint8_t infomask;

    if( memfrs_get_virmem_struct_content( cpu, cr3, (uint8_t*)&pointercount, sizeof(pointercount), handle_table_entry_ptr, "_OBJECT_HEADER", 1, "#PointerCount") !=-1 )
        if( pointercount > 0x1000000 || pointercount < 0 )
            return 0;

    if( memfrs_get_virmem_struct_content( cpu, cr3, (uint8_t*)&handlecount, sizeof(handlecount), handle_table_entry_ptr, "_OBJECT_HEADER", 1, "#HandleCount") !=-1 )
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
    if( memfrs_get_virmem_struct_content( cpu, cr3, (uint8_t*)&infomask, sizeof(infomask), handle_table_entry_ptr, "_OBJECT_HEADER", 1, "#InfoMask") !=-1 )
        if( infomask > 0x7f )
            return 0;

    // the range of type index is 2 to about 50 ,
    // lower bound is 2, but higher bound depending on Windows version
    if( memfrs_get_virmem_struct_content( cpu, cr3, (uint8_t*)&typeindex, sizeof(typeindex), handle_table_entry_ptr, "_OBJECT_HEADER", 1, "#TypeIndex") !=-1 )
        if( typeindex >=50 || typeindex < 1 )
            return 0;

    return 1;
}



/*******************************************************************
static void do_table_level_0( uint64_t tablecode_ptr, CPUState *cpu )

Find handle table entry in level 0 from handle table

INPUT:     level1_index              the index of level 1
           level0_ptr,               the address of level 0
           uint64_t cr3,             the current cr3
           json_object *process,     the json structure of one process for json structure handles
           CPUState *cpu,            the pointer to current cpu
OUTPUT:    void
*******************************************************************/
static void do_table_level_0( int level1_index, uint64_t level0_ptr, uint64_t cr3, json_object *process, CPUState *cpu )
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

        do_table_entry( grantedaccess, i+level1_index*256, handle_table_entry_ptr, cr3, process, cpu );
    }
}



/*******************************************************************
static void do_table_level_1( uint64_t tablecode_ptr, CPUState *cpu )

Find level 0 in level 1 from handle table

INPUT:     int level2_index,         the index of level 2 table
           level1_ptr,              the address of level 1
           uint64_t cr3,             the current cr3
           json_object *process,     the json structure of one process for json structure handles
           CPUState *cpu,            the pointer to current cpu
OUTPUT:    void
*******************************************************************/
static void do_table_level_1( int level2_index, uint64_t level1_ptr, uint64_t cr3, json_object *process, CPUState *cpu )
{
    int i=0;
    uint64_t level0_ptr;

    while( i<512 && cpu_memory_rw_debug( cpu, level1_ptr, (uint8_t*)&level0_ptr, sizeof(level0_ptr), 0) !=-1 ){
        if(level0_ptr==0x0)
            break;
        do_table_level_0( i+level2_index*256, level0_ptr, cr3, process, cpu );
        i = i+1;
        level1_ptr = level1_ptr+0x8;
    }
}



/*******************************************************************
static void do_table_level_2( uint64_t level2_ptr, uint64_t cr3, json_object *process, CPUState *cpu )

Find level 1 in level 2 from handle table

INPUT:     level2_ptr,               the address of level 2 table
           uint64_t cr3,             the current cr3
           json_object *process,     the json structure of one process for json structure handles
           CPUState *cpu,            the pointer to current cpu
OUTPUT:    void
*******************************************************************/
static void do_table_level_2( uint64_t level2_ptr, uint64_t cr3, json_object *process, CPUState *cpu )
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



/*******************************************************************
int memfrs_enum_proc_list( uint64_t kpcr_ptr, CPUState *cpu )

Eumerate the running process

INPUT:     uint64_t target_cr3       the target cr3. if target_cr3=0x0, stored handles of all process
           uint64_t kpcr_ptr,        the address of _KPCR struct
           CPUState *cpu,            the pointer to current cpu
           json_object *handles      the json structure of handles
OUTPUT:    int,                      return 0 if sucess, and not 0 otherwise
*******************************************************************/
int memfrs_enum_proc_handles( uint64_t target_cr3, uint64_t kpcr_ptr, CPUState *cpu, json_object *handles )
{
    json_object *process;
    char str_cr3[17];

    uint64_t cr3,
             kthread_ptr,
             eprocess_ptr_init = 0,
             eprocess_ptr,
             tablecode_ptr;

    json_object *jeprocess = NULL,
                *jobject_header = NULL;

    uint8_t buf[256];

    int offset_entry_list_to_eprocess = 0;
    field_info* f_info = NULL;

    int table_level;


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
    // Read the concrete memory value of kthread_ptr(CurrentThread) via _KPCR address
    memfrs_get_virmem_struct_content( cpu, 0, (uint8_t*)&kthread_ptr, sizeof(kthread_ptr), kpcr_ptr, "_KPCR", 2, "#Prcb", "#CurrentThread");

    // Read the concrete memory value of PROCESS via CurrentThread
    // Get the first PROCESS
    memfrs_get_virmem_struct_content( cpu, 0, (uint8_t*)&eprocess_ptr, sizeof(eprocess_ptr), kthread_ptr, "_KTHREAD", 1, "#Process");

    // Retrieve the _EPROCESS structure
    jeprocess = memfrs_q_struct("_EPROCESS");
    // Query ActiveProcessLinks in _EPROCESS
    f_info = memfrs_q_field(jeprocess, "ActiveProcessLinks");
    // Calculate Entry List offset from eprocess
    offset_entry_list_to_eprocess = f_info->offset;
    memfrs_close_field(f_info);

    // Retrieve the _EPROCESS structure
    jobject_header = memfrs_q_struct("_OBJECT_HEADER");
    // Query ActiveProcessLinks in _EPROCESS
    f_info = memfrs_q_field(jobject_header, "Body");
    // Calculate Entry List offset from eprocess
    g_body_to_object_header = f_info->offset;
    memfrs_close_field(f_info);


    // Start iteration process list
    eprocess_ptr_init = eprocess_ptr;

    do {
        //Read CR3 & Process name
        memfrs_get_virmem_struct_content( cpu, 0, (uint8_t*)&cr3, sizeof(cr3), eprocess_ptr, "_EPROCESS", 2, "#Pcb", "#DirectoryTableBase");
        memfrs_get_virmem_struct_content( cpu, 0, (uint8_t*)buf, sizeof(buf), eprocess_ptr, "_EPROCESS", 1, "#ImageFileName");

        memset(str_cr3, 0, 17);
        sprintf(str_cr3, "%"PRIx64, cr3);
        if( cr3!=0 ){

            if( target_cr3==0x0 || target_cr3==cr3 ){

                process = json_object_new_object();

                memfrs_get_virmem_struct_content( cpu, cr3, (uint8_t*)&tablecode_ptr, sizeof(tablecode_ptr), eprocess_ptr, "_EPROCESS", 2, "*ObjectTable", "#TableCode");
                table_level = tablecode_ptr & 0x3;
                tablecode_ptr = tablecode_ptr & 0xfffffffffffffffc ;

                // table level == 0 means we are at the bottom level and this is a table of _HANDLE_TABLE_ENTRY
                // otherwise, it means we are a table of pointers to lower tables.
                if( table_level == 0 )
                    do_table_level_0(0, tablecode_ptr, cr3, process, cpu);
                
                else if ( table_level == 1 )
                    do_table_level_1(0, tablecode_ptr, cr3, process, cpu);
                
                else 
                    do_table_level_2(tablecode_ptr, cr3, process, cpu);

                json_object_object_add(handles, str_cr3, process);

            }
        }

        // Read next entry
        memfrs_get_virmem_struct_content( cpu, 0, (uint8_t*)&eprocess_ptr, sizeof(eprocess_ptr), eprocess_ptr, "_EPROCESS", 2, "#ActiveProcessLinks", "*Blink");
        // Substract entry_list offset to find base address of eprocess
        eprocess_ptr = eprocess_ptr-offset_entry_list_to_eprocess;

    }while( eprocess_ptr != eprocess_ptr_init );

    return 0;
}
