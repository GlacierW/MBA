/*
 *  MBA Mmemory Forensic unit testing
 *
 *  Copyright (c) 2016 Chong-kuan, Chen
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


#ifndef CPU_COMMON_H
#define CPU_COMMON_H 1

#include "../../../include/utlist.h"

/*
#include "../../../json-c/json.h"
*/


#define X86_CPU(obj) ((X86CPU*)obj)


/*
typedef uint64_t hwaddr;

typedef struct CPUState{
} CPUState;

typedef struct Monitor{
} Monitor;
*/

#define CPU_NB_REGS 16

#define R_EAX 0
#define R_ECX 1
#define R_EDX 2
#define R_EBX 3
#define R_ESP 4
#define R_EBP 5
#define R_ESI 6
#define R_EDI 7

typedef unsigned long target_ulong;


typedef struct CPUX86State {
    ulong cr[5];
    target_ulong regs[CPU_NB_REGS];
} CPUX86State;


typedef struct X86CPU{
    CPUX86State env;
} X86CPU;


typedef struct CPUArchState {

} CPUArchState;
/*
void* current_cpu;
*/

#endif
