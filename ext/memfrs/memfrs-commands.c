/*
 *  MBA Virtual Machine Memory Forensic qemu command implementation
 *
 *  Copyright (c)   2012 Chiwei Wang
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

#include "ext/memfrs/memfrs-commands.h"
#include "ext/memfrs/memfrs.h"

#include "qmp-commands.h"

/******************************************************************
* PURPOSE : Load the data structure's information into 
            g_struct_info from json file of sdb_path
******************************************************************/
void do_load_structures(Monitor *mon, const QDict *qdict)
{
    const char* sdb_path = qdict_get_str(qdict, "sdb");
    memfrs_load_structs(sdb_path);
}

/******************************************************************
* PURPOSE : List the running process
******************************************************************/
void do_process_list(Monitor *mon, const QDict *qdict)
{
    CPUState *cpu, *thiscpu=NULL;    

    // Find the first CPU
    CPU_FOREACH(cpu)
    {
        monitor_printf(mon, "%p\n", cpu);
        thiscpu = cpu;
        break;
    }

    memfrs_enum_proc_list(g_kpcr_ptr, thiscpu);
}
