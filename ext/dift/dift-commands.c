#include "qemu-common.h"
#include "monitor/monitor.h"
#include "include/exec/cpu-common.h"

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
