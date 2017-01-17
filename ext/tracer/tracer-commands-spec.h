/*
 *  MBA Virtual Machine Memory Forensic qemu command specification
 *
 *  Copyright (c)   2016 Chongkuan Chen
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

{
        .name = "enable_tracer",
        .args_type  = "tracer_uid:i",
        .params     = "tracer_uid",
        .help       = "enalbing instruction trace with given uid number",
        .mhandler.cmd = do_enable_tracer,
},
{
        .name = "inst_tracer",
        .args_type  = "label:s,cr3:l,is_kernel:b",
        .params     = "label cr3 is_kernel",
        .help       = "create tracer for instruction trace, specify cr3 for pre-process trace, 0 for universal trace. is_kernel denotes kernel trace or not",
        .mhandler.cmd = do_inst_tracer,
},
{
        .name = "list_tracer",
        .args_type  = "",
        .params     = "",
        .help       = "list all tracer",
        .mhandler.cmd = do_list_tracer,
},

