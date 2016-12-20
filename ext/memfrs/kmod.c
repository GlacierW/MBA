/*
 *  MBA Kernel Module Introspection Implementation
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

#if !defined(CONFIG_MEMFRS_TEST)
#include "qemu-common.h"
#endif

#include "memfrs.h"
#include "memfrs-priv.h"
#include "kmod.h"

#define SIZE_OF_POOL_HEADER 0x10

/*********************************************************************************
void memfrs_scan_module(CPUState *cpu)

Scan the whole physical memory for MmLd Module tag, and list all the module name
in atdout.

INPUT:  CPUState *cpu            pointer to current cpu

OUTPUT: void                     
**********************************************************************************/

UT_icd module_icd = {sizeof(kernel_module), NULL, NULL, NULL };
UT_array*  memfrs_scan_module(CPUState *cpu)
{
    uint64_t i ;
    uint8_t* module_tag = (uint8_t*)malloc(strlen(POOL_TAG_MODULE));
    uint8_t buf[SIZEOFUNICODESTRING];
    int offset_tag, offset_fullname, offset_basename;
    UT_array *module_list;

    printf("Scan for pattern %s\n", POOL_TAG_MODULE);

    // Check if ds metadata is already loaded
    if( memfrs_check_struct_info() == 0)
    {
        printf("Data structure information is not loaded\n");
        return NULL;
    }

    json_object* jobj = memfrs_q_struct("_POOL_HEADER");
    field_info* f_info = NULL;
    f_info = memfrs_q_field(jobj, "PoolTag");
    offset_tag = f_info->offset;
    memfrs_close_field(f_info);

    jobj = memfrs_q_struct("_LDR_DATA_TABLE_ENTRY");
    f_info = memfrs_q_field(jobj, "FullDllName");
    offset_fullname = f_info->offset;
    memfrs_close_field(f_info);

    f_info = memfrs_q_field(jobj, "BaseDllName");
    offset_basename = f_info->offset;
    memfrs_close_field(f_info);

    utarray_new( module_list, &module_icd);

    //Scan whole physical memory
    for(i = 0; i < MAXMEM-strlen(POOL_TAG_MODULE); i++)
    {
        // Read tag
        cpu_physical_memory_read(i, module_tag, strlen(POOL_TAG_MODULE));
        if(memcmp( module_tag, POOL_TAG_MODULE, strlen(POOL_TAG_MODULE))==0)
        {
            printf( "pattern found %lx\n", i);
            // Retrieve whole path
            cpu_physical_memory_read(i- offset_tag+ SIZE_OF_POOL_HEADER+ offset_fullname, buf, SIZEOFUNICODESTRING);
            char* fullname = parse_unicode_str(buf, cpu);

            cpu_physical_memory_read(i- offset_tag+ SIZE_OF_POOL_HEADER+ offset_basename, buf, SIZEOFUNICODESTRING);
            char* basename = parse_unicode_str(buf, cpu);

            if(basename == NULL || fullname == NULL)
                continue;           
 
            kernel_module* kmod = (kernel_module*)malloc(sizeof(kernel_module));

            strcpy(kmod->fullname, fullname);
            strcpy(kmod->basename, basename);
            
            kmod->base = i;
            utarray_push_back(module_list, kmod);
        }
    }
    return module_list;
}

