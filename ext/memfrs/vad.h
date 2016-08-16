/*
 *  MBA VAD Introspection Header File
 *
 *  Copyright (c)   2016 ChongKuan Chen
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
#ifndef __MEMFRS_VAD_H__
#define __MEMFRS_VAD_H__

#include "qom/cpu.h"

typedef enum{
    VadNone,
    VadDevicePhysicalMemory,
    VadImageMap,
    VadAwe,
    VadWriteWatch,
    VadLargePages,
    VadRotatePhysical,
    VadLargePageSection
} MI_VAD_TYPE_ENUM;

typedef enum{
    PAGE_EXECUTE = 0x10,
    PAGE_EXECUTE_READ = 0x20,
    PAGE_EXECUTE_READWRITE = 0x40,
    PAGE_EXECUTE_WRITECOPY = 0x80,
    PAGE_NOACCESS = 0x01,
    PAGE_READONLY = 0x02,
    PAGE_READWRITE = 0x04,
    PAGE_WRITECOPY = 0x08,
    PAGE_GUARD = 0x100,
    PAGE_NOCACHE = 0x200,
    PAGE_WRITECOMBINE = 0x400,
} PAGE_PERMISSION;

static int const MmProtectToValue[32] = {
        PAGE_NOACCESS,
        PAGE_READONLY,
        PAGE_EXECUTE,
        PAGE_EXECUTE_READ,
        PAGE_READWRITE,
        PAGE_WRITECOPY,
        PAGE_EXECUTE_READWRITE,
        PAGE_EXECUTE_WRITECOPY,
        PAGE_NOACCESS,
        PAGE_NOCACHE | PAGE_READONLY,
        PAGE_NOCACHE | PAGE_EXECUTE,
        PAGE_NOCACHE | PAGE_EXECUTE_READ,
        PAGE_NOCACHE | PAGE_READWRITE,
        PAGE_NOCACHE | PAGE_WRITECOPY,
        PAGE_NOCACHE | PAGE_EXECUTE_READWRITE,
        PAGE_NOCACHE | PAGE_EXECUTE_WRITECOPY,
        PAGE_NOACCESS,
        PAGE_GUARD | PAGE_READONLY,
        PAGE_GUARD | PAGE_EXECUTE,
        PAGE_GUARD | PAGE_EXECUTE_READ,
        PAGE_GUARD | PAGE_READWRITE,
        PAGE_GUARD | PAGE_WRITECOPY,
        PAGE_GUARD | PAGE_EXECUTE_READWRITE,
        PAGE_GUARD | PAGE_EXECUTE_WRITECOPY,
        PAGE_NOACCESS,
        PAGE_WRITECOMBINE | PAGE_READONLY,
        PAGE_WRITECOMBINE | PAGE_EXECUTE,
        PAGE_WRITECOMBINE | PAGE_EXECUTE_READ,
        PAGE_WRITECOMBINE | PAGE_READWRITE,
        PAGE_WRITECOMBINE | PAGE_WRITECOPY,
        PAGE_WRITECOMBINE | PAGE_EXECUTE_READWRITE,
        PAGE_WRITECOMBINE | PAGE_EXECUTE_WRITECOPY
};

extern int parse_mmvad_node(uint64_t mmvad_ptr, CPUState *cpu);
extern void traverse_vad_tree(uint64_t eprocess_ptr, CPUState *cpu);
#endif
