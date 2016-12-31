/*
 *  MBA Tracer qemu command implementation
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

#include <stdio.h>

#include "qemu-common.h"
#include "monitor/monitor.h"

#include "ext/tracer/tracer-commands.h"
#include "ext/tracer/tracer.h"


void do_enable_tracer(Monitor *mon, const QDict *qdict)
{
    tracer_enable_tracer();
}

void do_inst_tracer(Monitor *mon, const QDict *qdict)
{
    const char* label = qdict_get_str(qdict, "label");
    uint64_t cr3 = qdict_get_int(qdict, "cr3");
    tracer_add_process(cr3, label, NULL); 
}

void do_list_tracer(Monitor *mon, const QDict *qdict)
{
    tracer_list_callback();
}
