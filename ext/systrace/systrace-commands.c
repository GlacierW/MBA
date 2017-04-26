/*
 *  MBA System Call Tracer qemu command implementation
 *
 *  Copyright (c)   2016 Hao Li
 *                  2016 Chiawei Wang
 *                  2016 Chongkuan Chen
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

#include "systrace-commands.h"
#include "systrace.h"
#include "systrace_priv.h"

void do_add_systrace(Monitor *mon, const QDict *qdict)
{
    const char* label = qdict_get_str(qdict, "label");
    uint64_t cr3 = qdict_get_int(qdict, "cr3"); 
    int syscall_num = qdict_get_int(qdict, "sycall_num");
    int handle = -1;
    monitor_printf(mon, "Create systracer '%s': cr3 %016lx, syscall %d\n", label, cr3, syscall_num);
    handle = systrace_add( cr3, syscall_num, cb_log_syscall_info, NULL );
    monitor_printf(mon, "Creation finished, hadle = %d\n", handle);
}

void do_delete_systrace(Monitor *mon, const QDict *qdict)
{
    int id = qdict_get_int(qdict, "id");
    int ret = 0;
    ret = systrace_delete(id);
    monitor_printf(mon, "Delete systracer id %d ret %d \n", id, ret);
}
