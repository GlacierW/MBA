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
#include "ext/tracer/tracer-priv.h"


void do_enable_tracer(Monitor *mon, const QDict *qdict)
{
    int uid = qdict_get_int(qdict, "tracer_uid");
    tracer_enable_tracer(uid);
}

// if cr3 != 0 => process tracer
// if cr3 == 0 && isKernel is true => kernel tracer
// if cr3 == 0 && isKernel is false => universal tracer

void do_inst_tracer(Monitor *mon, const QDict *qdict)
{
    const char* label = qdict_get_str(qdict, "label");
    uint64_t cr3 = qdict_get_int(qdict, "cr3");
    bool is_kernel = qdict_get_bool(qdict, "is_kernel");
    tracer_add_inst_tracer(cr3, label, is_kernel, NULL); 
}

void do_block_tracer(Monitor *mon, const QDict *qdict)
{
    const char* label = qdict_get_str(qdict, "label");
    uint64_t cr3 = qdict_get_int(qdict, "cr3");
    bool is_kernel = qdict_get_bool(qdict, "is_kernel");
    tracer_add_block_tracer(cr3, label, is_kernel, NULL);
}

void do_list_tracer(Monitor *mon, const QDict *qdict)
{
    tracer_list_callback();
}
