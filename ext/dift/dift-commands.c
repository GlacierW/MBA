#include "qemu-common.h"
#include "monitor/monitor.h"
#include "include/exec/cpu-common.h"
#include "exec/cpu-all.h"

#include "ext/dift/dift-commands.h"
#include "ext/dift/dift.h"

void do_memory_contaminate(Monitor *mon, const QDict *qdict)
{
    uint64_t target_addr = qdict_get_int(qdict, "addr");
    uint64_t target_range = qdict_get_int(qdict, "range");
    uint64_t contaminate_tag = qdict_get_int(qdict, "contaminate");

    monitor_printf(mon, "Start tainting( addr %ld range %ld contaminate %ld)\n", target_addr, target_range, contaminate_tag);
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

    monitor_printf(mon, "Taint addr %ld length %ld\n", target_addr, target_length);

    for(i = 0 ; i < target_length ; i++) {
        monitor_printf( mon, "%02x|%02x, ",
                buf[i], dift_get_memory_dirty(target_addr + i));
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
