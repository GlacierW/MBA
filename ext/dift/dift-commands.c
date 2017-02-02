/*
 *  HMP command implementation of the MBA DIFT
 *
 *  Copyright (c)   2016 Chiawei Wang
 *                  2016 Chongkuan Chen
 *                  2016 Hao Li
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
#include "monitor/monitor.h"
#include "include/exec/cpu-common.h"
#include "exec/cpu-all.h"

#include "ext/dift/dift-commands.h"
#include "ext/dift/dift.h"

void do_set_memory_taint(Monitor *mon, const QDict *qdict)
{
    uint64_t target_addr = qdict_get_int(qdict, "addr");
    uint64_t target_range = qdict_get_int(qdict, "range");
    uint64_t contaminate_tag = qdict_get_int(qdict, "contaminate");

    monitor_printf(mon, "Start tainting( addr %"PRIx64" range %ld contaminate %ld)\n", target_addr, target_range, contaminate_tag);
    dift_contaminate_memory_or(target_addr, target_range, contaminate_tag & 0xff);
    monitor_printf(mon, "Taint finish\n");
}

void do_show_memory_taint_map(struct Monitor *mon, const struct QDict *qdict)
{
    uint64_t target_addr = qdict_get_int(qdict, "addr");
    uint64_t target_length = qdict_get_int(qdict, "len");
    int i;
    uint8_t* buf = (uint8_t*)malloc(target_length); 
    cpu_physical_memory_read(target_addr, buf, target_length);
    if(buf == NULL){
        monitor_printf(mon, "Cannot allocate memory for do_show_memory_taint_map()\n");
        return;
    }

    monitor_printf(mon, "Taint addr %"PRIx64" length %ld\n", target_addr, target_length);

    for(i = 0 ; i < target_length ; i++) {
        monitor_printf( mon, "%02x|%02x, ",
                buf[i], dift_get_memory_dirty(target_addr + i));
        if((i & 0xf) == 0xf)
            monitor_printf(mon, "\n");
    }
}

void do_set_disk_taint(Monitor *mon, const QDict *qdict)
{
    uint64_t target_addr = qdict_get_int(qdict, "addr");
    uint64_t target_range = qdict_get_int(qdict, "range");
    uint64_t contaminate_tag = qdict_get_int(qdict, "contaminate");

    monitor_printf(mon, "Start tainting( addr %"PRIx64" range %ld contaminate %ld)\n", target_addr, target_range, contaminate_tag);
    dift_contaminate_disk_or(target_addr, target_range, contaminate_tag & 0xff);
    monitor_printf(mon, "Taint finish\n");
}

void do_show_disk_taint_map(struct Monitor *mon, const struct QDict *qdict)
{
    uint64_t target_addr = qdict_get_int(qdict, "addr");
    uint64_t target_length = qdict_get_int(qdict, "len");
    int i;

    monitor_printf(mon, "Taint addr %"PRIx64" length %ld\n", target_addr, target_length);

    for(i = 0 ; i < target_length ; i++) {
        monitor_printf( mon, "%02x, ",
                 dift_get_disk_dirty(target_addr + i));
        if((i & 0xf) == 0xf)
            monitor_printf(mon, "\n");
    }
}

void do_get_physic_address(struct Monitor *mon, const struct QDict *qdict)
{
    uint64_t target_cr3		= qdict_get_int(qdict, "cr3");
    uint64_t target_addr    = qdict_get_int(qdict, "addr");
    X86CPU *cpu				= X86_CPU(ENV_GET_CPU((CPUArchState*)mba_mon_get_cpu()));
    hwaddr page             = target_addr & TARGET_PAGE_MASK;
	
    //XXX(misterlihao@gmail.com):Only one phase copied. Should be fully copied to resist changes.
    X86CPU copied_cpu;
    memcpy(&copied_cpu, cpu, sizeof(copied_cpu));
    copied_cpu.env.cr[3] = target_cr3;
    
    hwaddr phys_page = cpu_get_phys_page_debug((CPUState*)&copied_cpu, page);
    if (phys_page == -1) {
        monitor_printf(mon, "Cannot find physic page\n");
		return;
    }

    hwaddr phys_addr = phys_page + (target_addr & ~TARGET_PAGE_MASK);
    monitor_printf(mon, "physic address = %p\n", (void*)phys_addr);
}

void do_enable_dift(struct Monitor *mon, const struct QDict *qdict )
{
    if( dift_is_enabled() )
        return;
    dift_enable();
}

void do_disable_dift(struct Monitor *mon, const struct QDict *qdict)
{
    if( !dift_is_enabled() )
        return;
    dift_disable();
}

void do_dift_status(struct Monitor* mon, const struct QDict *qdict)
{
    if( dift_is_enabled() )
        monitor_printf( mon, "DIFT enabled\n" );
    else
        monitor_printf( mon, "DIFT disabled\n" );
}
