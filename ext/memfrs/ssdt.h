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


// process list
typedef struct ssdt_list_st{
    int      index;             // system call index, from 0 to number of system call
    uint64_t address;           // the system call actually virtual address
    char *system_call_name;     // system call name
}ssdt_list_st;



/*******************************************************************
extern UT_array* memfrs_enum_ssdt_list( uint64_t kpcr_ptr, CPUState *cpu );

Eumerate the ssdt

INPUT:     uint64_t kpcr_ptr         kpcr address
           CPUState *cpu,            the pointer to current cpu
OUTPUT:    UT_array*                 return a UT_array with ssdt data
*******************************************************************/
extern UT_array* memfrs_enum_ssdt_list( uint64_t kpcr_ptr, CPUState *cpu );
