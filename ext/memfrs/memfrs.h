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

//represent the field in the data structure
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

//public API 
extern bool memfrs_check_struct_info(void);
extern int memfrs_load_structs( const char* type_filename);
extern bool memfrs_kpcr_self_check( uint64_t seg_gs_cpl0 );
extern int  memfrs_enum_proc_list( uint64_t seg_gs_cpl0, CPUState *cp );
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
extern UT_array*  memfrs_scan_module(CPUState *cpu);
extern UT_array* memfrs_traverse_vad_tree(uint64_t eprocess_ptr, CPUState *cpu);
extern reverse_symbol* memfrs_build_gvar_lookup_map(void);
extern char* memfrs_get_symbolname_via_address(reverse_symbol* rsym_tab, int offset);
extern int memfrs_free_reverse_lookup_map(reverse_symbol* rsym_tab);
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
/// \param len              length of centent to read
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
/*
extern void parse_unicode_strptr(uint64_t ustr_ptr, CPUState *cpu);
extern void parse_unicode_str(uint8_t* ustr, CPUState *cpu);
extern void hexdump(Monitor *mon, uint8_t* buf, size_t length);
*/
#endif

