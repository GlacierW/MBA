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

#include "../../../include/utarray.h"
#include "../../../json-c/json.h"

#define X86_CPU(obj) (obj)

typedef uint64_t hwaddr;

typedef struct CPUState{
} CPUState;

typedef struct Monitor{
} Monitor;

typedef struct CPUX86State {
    ulong cr[5];
} CPUX86State;

typedef struct X86CPU{
    CPUX86State env;
} X86CPU;




void* current_cpu;


#endif
