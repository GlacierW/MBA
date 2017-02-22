/*
 *  MBA Virtual Machine Memory Introspection header
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
#ifndef __MEMFRS_H__
#define __MEMFRS_H__

#include <stdbool.h>
#include <inttypes.h>

#if !defined(CONFIG_MEMFRS_TEST)
#include "qom/cpu.h"
#include "json-c/json.h"
#include "include/utarray.h"
#include "include/uthash.h"
#endif

#define STRLEN 128
#define SIZEOFUNICODESTRING 0x10



typedef enum MEMFRS_ERRNO{
    MEMFRS_ERR_NOT_LOADED_GLOBAL_STRUCTURE,
    MEMFRS_ERR_NOT_LOADED_STRUCTURE,
    MEMFRS_ERR_NOT_FOUND_KPCR,
    MEMFRS_ERR_INVALID_CPU
}MEMFRS_ERRNO;

extern MEMFRS_ERRNO memfrs_errno;



// represent the field in the data structure
typedef struct field_info
{
    int offset;                    // field offset related to data structure  
    char type_name[STRLEN];
    int type_size;                 // the size of the field
    bool is_pointer;               // idicate if the field is pointer
    json_object* jobject_type;     // the json object to the field type
} field_info;

typedef struct reverse_symbol {
    int offset;            /* we'll use this field as the key */
    char* symbol;
    UT_hash_handle hh; /* makes this structure hashable */
} reverse_symbol;

extern uint64_t g_kpcr_ptr;



// process list
typedef struct process_list_st{
    uint64_t eprocess;
    uint64_t CR3;
    uint64_t pid;
    char *full_file_path;
}process_list_st;



// handles data sturcture
typedef struct handles_node_st{
    int handle_table_entry_index;
    uint64_t handle_table_entry_address;
    uint64_t grantedaccess;
    char *type;
    char *detail;
}handles_node_st;

typedef struct UT_handles{
    uint64_t CR3;
    uint64_t eprocess;
    uint64_t pid;
    char *imagename;
    UT_array *handles_node;
}handles_st;



// network data structure
typedef struct network_state{
    uint64_t offset;
    const char* protocol;
    uint64_t pmem;
    uint64_t eprocess;
    char* file_name;
    uint64_t pid;
    const char* state;
    char* local_addr;
    char* foreign_addr;
    char* time;
}network_state;



//public API 
extern bool memfrs_check_struct_info(void);
extern bool memfrs_check_network_struct_info(void);
extern int memfrs_load_structs( const char* type_filename);
extern int memfrs_load_network_structs( const char* type_filename);
extern bool memfrs_kpcr_self_check( uint64_t seg_gs_cpl0 );
extern UT_array* memfrs_enum_proc_list( uint64_t seg_gs_cpl0, CPUState *cp );
extern json_object* memfrs_q_struct(const char* ds_name);
extern field_info* memfrs_q_field( json_object* struc, const char* field_name  );
extern int memfrs_close_field(field_info* field);
extern UT_array* memfrs_scan_virmem( CPUState *cpu, uint64_t start_addr, uint64_t end_addr, const char* pattern, int length );
extern UT_array* memfrs_scan_phymem( uint64_t start_addr, uint64_t end_addr, const char* pattern, int length );
extern int memfrs_get_virmem_content( CPUState *cpu, uint64_t cr3, uint64_t target_addr, uint64_t target_length, uint8_t* buf);
extern int memfrs_load_globalvar( const char* type_filename);
extern json_object* memfrs_q_globalvar(const char* gvar_name);
extern int64_t memfrs_gvar_offset(json_object* gvarobj);
extern uint64_t memfrs_find_nt_kernel_base(CPUState* cpu);
extern uint64_t memfrs_get_nt_kernel_base(void);
extern UT_array* memfrs_scan_module(CPUState *cpu);
extern UT_array* memfrs_traverse_vad_tree(uint64_t eprocess_ptr, CPUState *cpu);
extern reverse_symbol* memfrs_build_gvar_lookup_map(void);
extern char* memfrs_get_symbolname_via_address(reverse_symbol* rsym_tab, int offset);
extern int memfrs_free_reverse_lookup_map(reverse_symbol* rsym_tab);
extern void* memfrs_get_kernel_info(void);
extern int memfrs_gen_pdb_profiles(const char* profile_dir);
extern int memfrs_display_type(CPUState *cpu, uint64_t addr, const char* struct_name);
/// get field content of some struct in virtual memory
///
/// e.g. To get content of following structure,
/// which struct A *p = 0xdeadbeef in guest virtual memory:
/// struct A {
///     struct B field1;
/// };
/// struct B {
///     struct C *field2;
/// };
/// struct C {
///     uint64_t field3;
/// };
///
/// memfrs_get_virmem_struct_content(cpu, cr3, buffer, sizeof(uint64_t), 0xdeadbeef, "A",
///         3, "#field1", "*field2", "#field3");
/// 
/// \param cpu              the running cpu
/// \param cr3              cr3 register of target virtual memory space
/// \param buffer           output buffer
/// \param len              length of content to read
/// \param struct_addr      the virtual address of the target structure
/// \param struct_type_name the type name of the target structure
/// \param depth            the field access chain depth
/// \param ...              field names in the query chain
/// 
/// return -1 on error, 0 on success
extern int memfrs_get_virmem_struct_content(
        CPUState   *cpu,
        uint64_t    cr3,
        uint8_t    *buffer,
        int         len,
        uint64_t    struct_addr,
        const char *struct_type_name,
        int         depth,
        ...);
extern int memfrs_get_phy_virmem_struct_content(
        CPUState   *cpu,
        uint64_t    cr3,
        uint8_t    *buffer,
        int         len,
        uint64_t    struct_addr,
        const char *phy_struct_type_name,
        const char *phy_field_name,
        const char *struct_type_name,
        int         depth,
        ...);
extern int memfrs_get_nested_field_offset(int *out, const char *struct_type_name, int depth, ...);

/*******************************************************************
extern UT_array* memfrs_enum_proc_handles( int target_type, uint64_t target, uint64_t kpcr_ptr, CPUState *cpu )

Eumerate the running process handles

INPUT:     int target_type,          searching type of handles
           uint64_t target,          searching target
           uint64_t kpcr_ptr,        the address of _KPCR struct
           CPUState *cpu,            the pointer to current cpu
OUTPUT:    UT_array*                 return a UT_array with handles data
*******************************************************************/
extern UT_array* memfrs_enum_proc_handles( int target_type, uint64_t target, uint64_t kpcr_ptr, CPUState *cpu );

/*******************************************************************
extern UT_array* memfrs_enum_proc_handles_detail( int target_type, const char* target, uint64_t kpcr_ptr, CPUState *cpu )

Eumerate the running process handles, expect for types and details

INPUT:     int target_type,          searching type of handles
           const char* target        searching target
           uint64_t kpcr_ptr,        the address of _KPCR struct
           CPUState *cpu,            the pointer to current cpu
OUTPUT:    UT_array*                 return a UT_array with handles data
*******************************************************************/
extern UT_array* memfrs_enum_proc_handles_detail( int target_type, const char* target, uint64_t kpcr_ptr, CPUState *cpu );

/*******************************************************************
extern UT_array* memfrs_enum_handles_types( uint64_t kpcr_ptr, CPUState *cpu )

Eumerate the handles types

INPUT:     uint64_t kpcr_ptr,        the address of _KPCR struct
           CPUState *cpu,            the pointer to current cpu
OUTPUT:    UT_array*                 return a UT_array with handles types
*******************************************************************/
extern UT_array* memfrs_enum_handles_types( uint64_t kpcr_ptr, CPUState *cpu );

/*********************************************************************************
UT_array memfrs_scan_network(CPUState *cpu)

Scan the whole physical memory for network pool tag, and list all the network state.

INPUT:  CPUState *cpu            pointer to current cpu
OUTPUT: UT_array*                return a UT_array with handles types
**********************************************************************************/
extern UT_array* memfrs_scan_network(CPUState *cpu);

/*
extern void parse_unicode_strptr(uint64_t ustr_ptr, CPUState *cpu);
extern void parse_unicode_str(uint8_t* ustr, CPUState *cpu);
extern void hexdump(Monitor *mon, uint8_t* buf, size_t length);
*/
#endif

