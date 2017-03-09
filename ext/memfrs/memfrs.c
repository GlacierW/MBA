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

#include <stdarg.h>

/* Global Variable */
uint64_t g_kpcr_ptr = 0;
json_object *g_struct_info = NULL;
json_object *g_globalvar_info = NULL;

MEMFRS_ERRNO memfrs_errno;

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

int A=0;
/*******************************************************************
int memfrs_load_structs( const char* type_filename)

Load the data structure information into g_struct_info.

INPUT:     const char* type_filename,  the filename of json data structure database
OUTPUT:    int,                        return 0 if sucess, and not 0 otherwise

*******************************************************************/
int memfrs_load_structs( const char* type_filename)
{
    json_object *struct_info = NULL, *test_obj = NULL;
    if(g_struct_info==NULL){
        g_struct_info = json_object_from_file(type_filename);
    }
    else{
        struct_info = json_object_from_file(type_filename);
        json_object_object_foreach(struct_info, key, val){
            json_object_object_get_ex(g_struct_info, key, &test_obj);
            if(test_obj!=NULL){
                printf("The json object with key %s has been overwride.\n", key);
            }
            json_object_object_add(g_struct_info, key, val);
        }
    }

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
    memfrs_get_virmem_struct_content( cpu, 0, (uint8_t*)&kthread_ptr, sizeof(kthread_ptr), kpcr_ptr, "_KPCR", 2, "#Prcb", "#CurrentThread");

    // Read the concrete memory value of PROCESS via CurrentThread
    // Get the first PROCESS
    memfrs_get_virmem_struct_content( cpu, 0, (uint8_t*)&eprocess_ptr, sizeof(eprocess_ptr), kthread_ptr, "_KTHREAD", 1, "#Process");

    // Assign process_list be a 'process_list_st' structure UTarray
    utarray_new( list, &proc_list_icd);


    // Start iteration process list
    eprocess_ptr_init = eprocess_ptr;

    do {

        //Read CR3 & Process name
        memfrs_get_virmem_struct_content( cpu, 0, (uint8_t*)&cr3, sizeof(cr3), eprocess_ptr, "_EPROCESS", 2, "#Pcb", "#DirectoryTableBase");
        memfrs_get_virmem_struct_content( cpu, 0, (uint8_t*)&processid, sizeof(processid), eprocess_ptr, "_EPROCESS", 1, "#UniqueProcessId");
        memfrs_get_virmem_struct_content( cpu, 0, (uint8_t*)file_name_buf, sizeof(file_name_buf), eprocess_ptr, "_EPROCESS", 1, "#ImageFileName");
        //printf( "0x%-20lx%-20lx%-5"PRId64" ", eprocess_ptr, cr3, processid );

        if( cr3 !=0 ){
            if ( memfrs_get_virmem_struct_content(cpu, cr3, (uint8_t*)&length, sizeof(length), eprocess_ptr, "_EPROCESS", 4, "*Peb", "*ProcessParameters", "#ImagePathName", "#Length") == -1 )
                length =0;
            if ( memfrs_get_virmem_struct_content(cpu, cr3, (uint8_t*)&buf_ptr, sizeof(buf_ptr), eprocess_ptr, "_EPROCESS", 4, "*Peb", "*ProcessParameters", "#ImagePathName", "*Buffer") == -1 )
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
        memfrs_get_virmem_struct_content( cpu, 0, (uint8_t*)&eprocess_ptr, sizeof(eprocess_ptr), eprocess_ptr, "_EPROCESS", 2, "#ActiveProcessLinks", "*Blink");
        // Substract entry_list offset to find base address of eprocess
        memfrs_get_nested_field_offset(&offset_entry_list_to_eprocess, "_EPROCESS", 1, "ActiveProcessLinks");
        eprocess_ptr = eprocess_ptr-offset_entry_list_to_eprocess;

    }while( eprocess_ptr != eprocess_ptr_init );

    return list;
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

    buf = (uint8_t*)malloc(max_length+2);
    str = (char*)malloc(max_length+1);
    memset(str, 0, max_length+1);
    cpu_memory_rw_debug( cpu, buf_ptr, buf, max_length, 0 );
    //Hardcode Unicode Parse
    //wcstombs(str, (const wchar_t *)buf, max_length);
    for(i=0; i<max_length;i+=2)
        str[i/2] = buf[i];   
    str[i] = 0x00;

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

static int memfrs_get_mem_struct_content(
        CPUState   *cpu,
        uint64_t    cr3,
        uint8_t    *buffer,
        int         len,
        uint64_t    struct_addr,
        const char *struct_type_name,
        int         depth,
        va_list vl) {
    // XXX: Now use extra char at beginning of field name to
    // indicate that the field is a pointer or not.
    // Should load and parse the structure file correctly instead.
    // XXX: assuming pointer has size of 8
    json_object *struct_type;
    field_info *info = NULL;
    int errcode;
    const char *field_name;

    struct_type = memfrs_q_struct(struct_type_name);
    if (struct_type == NULL)
        return -1;

    // Process field query
    while (depth--) {
        if (info && info->is_pointer) {
            errcode = memfrs_get_virmem_content(cpu, cr3, struct_addr, 8, (uint8_t*)&struct_addr);
            if (errcode == -1)
                return -1;
        }

        free(info);
        field_name = va_arg(vl, const char*);
        info = memfrs_q_field(struct_type, field_name+1);
        if (info == NULL)
            return -1;

        if (field_name[0] == '*')
            info->is_pointer = true;
        else
            info->is_pointer = false;

        struct_addr += info->offset;
        struct_type = info->jobject_type;
    }
    free(info);

    return memfrs_get_virmem_content(cpu, cr3, struct_addr, len, buffer);
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

    int errcode = 0;
    va_list vl;

    va_start(vl, depth);
    errcode = memfrs_get_mem_struct_content( cpu, cr3, (uint8_t*)buffer, len, struct_addr, struct_type_name, depth, vl);
    va_end(vl);

    return errcode;
}

int memfrs_get_phy_virmem_struct_content(
        CPUState   *cpu,
        uint64_t    cr3,
        uint8_t    *buffer,
        int         len,
        uint64_t    struct_addr,
        const char *phy_struct_type_name,
        const char *phy_field_name,
        const char *struct_type_name,
        int         depth,
        ...) {

    int errcode = 0;
    va_list vl;
    json_object *struct_type;
    field_info *info = NULL;

    struct_type = memfrs_q_struct(phy_struct_type_name);
    if (struct_type == NULL)
        return -1;

    info = memfrs_q_field(struct_type, phy_field_name);

    if(depth==1){
        cpu_physical_memory_read(struct_addr + info->offset , buffer, len);
        free(info);
        return 0;
    }
    else if(depth>1){
        cpu_physical_memory_read( struct_addr + info->offset , &struct_addr, 8);
        free(info);

        va_start(vl, depth);
        errcode = memfrs_get_mem_struct_content( cpu, cr3, (uint8_t*)buffer, len, struct_addr, struct_type_name, depth-1, vl);
        va_end(vl);

        return errcode;
    }
    else
        return -1;
}

int memfrs_get_nested_field_offset(int *out, const char *struct_type_name, int depth, ...) {
    json_object *struct_type;
    va_list vl;
    field_info *info = NULL;
    const char *field_name;
    int offset = 0;

    struct_type = memfrs_q_struct(struct_type_name);
    if (struct_type == NULL)
        return -1;

    va_start(vl, depth);
    // Process field query
    while (depth--) {
        field_name = va_arg(vl, const char*);
        info = memfrs_q_field(struct_type, field_name);
        if (info == NULL)
            return -1;

        struct_type = info->jobject_type;
        offset += info->offset;
        free(info);
    }
    va_end(vl);

    *out = offset;
    return 0;
}
