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


extern uint64_t g_kpcr_ptr;


typedef enum MEMFRS_ERRNO{
    MEMFRS_ERR_NOT_LOADED_GLOBAL_STRUCTURE,
    MEMFRS_ERR_NOT_LOADED_STRUCTURE,
    MEMFRS_ERR_NOT_FOUND_KPCR,
    MEMFRS_ERR_NOT_FOUND_GLOBAL_STRUCTURE,
    MEMFRS_ERR_NOT_FOUND_KERNEL_BASE,
    MEMFRS_ERR_MEMORY_READ_FAILED,
    MEMFRS_ERR_INVALID_CPU
}MEMFRS_ERRNO;

extern MEMFRS_ERRNO memfrs_errno;


// current thread datas
typedef struct current_thread
{
    uint64_t unique_thread;
    uint64_t unique_process;
    char* image_file_name;
} current_thread;



// represent the field in the data structure
typedef struct field_info
{
    int offset;                    // field offset related to data structure  
    char type_name[STRLEN];
    int type_size;                 // the size of the field
    bool is_pointer;               // idicate if the field is pointer
    int pointer_dereference_count;               // idicate if the field is pointer
    json_object* jobject_type;     // the json object to the field type
} field_info;

typedef struct reverse_symbol {
    int offset;            /* we'll use this field as the key */
    char* symbol;
    UT_hash_handle hh; /* makes this structure hashable */
} reverse_symbol;



// public API 
extern bool memfrs_check_struct_info(void);
extern bool memfrs_kpcr_self_check( uint64_t seg_gs_cpl0 );


/// get windows version
/// requirement : structure, global_structure
///
/// Version number  Operating system
///    -1.0         [ERROR] Need to check error code
///     0.0         [Unknown] Unknown version
///    10.0         Windows 10
///    10.0         Windows Server 2016
///     6.3         Windows 8.1
///     6.3         Windows Server 2012 R2
///     6.2         Windows 8
///     6.2         Windows Server 2012
///     6.1         Windows 7
///     6.1         Windows Server 2008 R2
///     6.0         Windows Server 2008
///     6.0         Windows Vista
///     5.2         Windows Server 2003 R2
///     5.2         Windows Server 2003
///     5.2         Windows XP 64-Bit Edition
///     5.1         Windows XP
///     5.0         Windows 2000
extern float memfrs_get_windows_version( uint64_t kpcr_ptr, CPUState *cpu );


/// get current thread
/// rteturn structure current_thread stored the thread datas
extern current_thread *memfrs_get_current_thread( CPUState *cpu );

extern bool memfrs_check_network_struct_info(void);
extern bool memfrs_check_globalvar_info(void);
extern int memfrs_load_structs( const char* type_filename);
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
/// memfrs_get_mem_struct_content(cpu, cr3, buffer, sizeof(uint64_t), 0xdeadbeef, "A", false,
///         3, "#field1", "*field2", "#field3");
/// 
/// \param cpu              the running cpu
/// \param cr3              cr3 register of target virtual memory space
/// \param buffer           output buffer
/// \param len              length of content to read
/// \param struct_addr      the virtual address of the target structure
/// \param bool             from_physical_memory,
/// \param struct_type_name the type name of the target structure
/// \param depth            the field access chain depth
/// \param ...              field names in the query chain
/// 
/// return -1 on error, 0 on success
extern int memfrs_get_mem_struct_content(
        CPUState   *cpu,
        uint64_t    cr3,
        uint8_t    *buffer,
        int         len,
        uint64_t    struct_addr,
        bool        from_physical_memory,
        const char *struct_type_name,
        int         depth,
        ...);
extern int memfrs_get_nested_field_offset(int *out, const char *struct_type_name, int depth, ...);


#endif
