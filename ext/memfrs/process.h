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
typedef struct process_list_st{
    uint64_t eprocess;          // the process' eprocess
    uint64_t CR3;               // the process' CR3
    uint64_t pid;               // the process' process id
    char *full_file_path;       // the process' full file path (not include command)
                                //     if full file path can't find in data struct
                                //     only stored the file name.
}process_list_st;



/*******************************************************************
extern UT_array* memfrs_enum_proc_list( uint64_t kpcr_ptr, CPUState *cpu );

Eumerate the running process

INPUT:     uint64_t kpcr_ptr         kpcr address
           CPUState *cpu,            the pointer to current cpu
OUTPUT:    UT_array*                 return a UT_array with process data
*******************************************************************/
extern UT_array* memfrs_enum_proc_list( uint64_t kpcr_ptr, CPUState *cpu );

