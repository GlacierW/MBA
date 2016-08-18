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


#include "qemu-common.h"
#include "ext/memfrs/memfrs.h"
#include "ext/memfrs/kmod.h"

/*********************************************************************************
void scan_module(CPUState *cpu)

Scan the whole physical memory for MmLd Module tag, and list all the module name
in atdout.

INPUT:  CPUState *cpu            pointer to current cpu

OUTPUT: void                     
**********************************************************************************/
void scan_module(CPUState *cpu)
{
    uint64_t i ;
    uint8_t* module_tag = (uint8_t*)malloc(strlen(POOL_TAG_MODULE));
    uint8_t buf[SIZEOFUNICODESTRING];
    printf("Scan for pattern %s\n", POOL_TAG_MODULE);

    //Scan whole physical memory
    for(i = 0; i < MAXMEM-strlen(POOL_TAG_MODULE); i++)
    {
        // Read tag
        cpu_physical_memory_read(i, module_tag, strlen(POOL_TAG_MODULE));
        if(memcmp( module_tag, POOL_TAG_MODULE, strlen(POOL_TAG_MODULE))==0)
        {
            printf( "pattern found %"PRIx64"\n", i);
            //TODO: Use ds query api instead
            // Retrieve whole path
            cpu_physical_memory_read(i-0x4+0x10+0x48, buf, SIZEOFUNICODESTRING);
            parse_unicode_str(buf, cpu);
  
            // Retrieve base name of kernel module
            cpu_physical_memory_read(i-0x4+0x10+0x58, buf, SIZEOFUNICODESTRING);
            parse_unicode_str(buf, cpu);
            printf("\n");
        }
    }

}

