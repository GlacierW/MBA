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

extern uint64_t g_kpcr_ptr;

//public API 
extern bool memfrs_check_struct_info(void);
extern int memfrs_load_structs( const char* type_filename);
extern bool memfrs_kpcr_self_check( uint64_t seg_gs_cpl0 );
extern int  memfrs_enum_proc_list( uint64_t seg_gs_cpl0, CPUState *cp );
extern json_object* memfrs_q_struct(const char* ds_name);
extern field_info* memfrs_q_field( json_object* struc, const char* field_name  );
extern int memfrs_close_field(field_info* field);
extern UT_array* memfrs_scan_virmem( CPUState *cpu, uint64_t start_addr, uint64_t end_addr, const char* pattern );
extern UT_array* memfrs_scan_phymem( uint64_t start_addr, uint64_t end_addr, const char* pattern );
extern void memfrs_get_virmem_content( CPUState *cpu, uint64_t cr3, uint64_t target_addr, uint64_t target_length, uint8_t* buf);
extern int memfrs_load_globalvar( const char* type_filename);
extern json_object* memfrs_q_globalvar(const char* gvar_name);
extern uint64_t memfrs_gvar_offset(json_object* gvarobj);
extern uint64_t memfrs_find_nt_kernel_base(CPUState* cpu);
extern uint64_t memfrs_get_nt_kernel_base(void);
extern void memfrs_scan_module(CPUState *cpu);
extern void memfrs_traverse_vad_tree(uint64_t eprocess_ptr, CPUState *cpu);

/*
extern void parse_unicode_strptr(uint64_t ustr_ptr, CPUState *cpu);
extern void parse_unicode_str(uint8_t* ustr, CPUState *cpu);
extern void hexdump(Monitor *mon, uint8_t* buf, size_t length);
*/
#endif

