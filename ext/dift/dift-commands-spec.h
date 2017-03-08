/*
 *  MBA DIFT QEMU command specification
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
{
        .name = "mba_set_mem_taint",
        .args_type  = "addr:l,range:l,contaminate:l",
        .params     = "addr range contaminate",
        .help       = "set memory byte(s) as tainted",
        .mhandler.cmd = do_set_memory_taint,
},
{
        .name = "mba_show_mem_taint",
        .args_type  = "addr:l,len:l",
        .params     = "addr len",
        .help       = "show memory taint map",
        .mhandler.cmd = do_show_memory_taint_map,
},
{
        .name = "mba_set_disk_taint",
        .args_type  = "addr:l,range:l,contaminate:l",
        .params     = "addr range contaminate",
        .help       = "set disk byte(s) as tainted",
        .mhandler.cmd = do_set_disk_taint,
},
{
        .name = "mba_show_disk_taint",
        .args_type  = "addr:l,len:l",
        .params     = "addr len",
        .help       = "Show disk taint map",
        .mhandler.cmd = do_show_disk_taint_map,
},
{
        .name = "mba_enable_dift",
        .args_type  = "",
        .params     = "",
        .help       = "enable DIFT taint analysis",
        .mhandler.cmd = do_enable_dift,
},
{
        .name = "mba_disable_dift",
        .args_type  = "",
        .params     = "",
        .help       = "disable DIFT taint analysis",
        .mhandler.cmd = do_disable_dift,
},
{
        .name = "mba_dift_status",
        .args_type  = "",
        .params     = "",
        .help       = "show DIFT status (enabled/disabled)",
        .mhandler.cmd = do_dift_status,
},

