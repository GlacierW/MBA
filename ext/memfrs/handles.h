/*
 *  MBA Virtual Machine Memory Introspection header
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


typedef enum PARSING_HANDLE_TYPE{
    PARSING_HANDLE_TYPE_ALL,
    PARSING_HANDLE_TYPE_CR3,
    PARSING_HANDLE_TYPE_EPROCESS,
    PARSING_HANDLE_TYPE_PID,
    PARSING_HANDLE_TYPE_TYPE,
    PARSING_HANDLE_TYPE_FULL_DETAIL,
    PARSING_HANDLE_TYPE_DETAIL
}PARSING_HANDLE_TYPE;

extern PARSING_HANDLE_TYPE parsing_handle_type;


// handles data sturcture
typedef struct handles_node_st{
    int handle_table_entry_index;           // the process' handle table entry index
    uint64_t handle_table_entry_address;    // the process' handle table entry address
    uint64_t grantedaccess;                 // the handle's granted access
    char *type;                             // the handle's type
    char *detail;                           // the handle's detail
}handles_node_st;

typedef struct UT_handles{
    uint64_t CR3;           // the process' CR3
    uint64_t eprocess;      // the process' eprocess
    uint64_t pid;           // the process' process id
    char *imagename;        // the process' file name
    UT_array *handles_node; // handles node, store the target process' handles
}handles_st;



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

