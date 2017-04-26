/*
    //$LogFile
 *  MBA System Call Tracer QEMU command specification 
 *
 *  Copyright (c)   2016 Chiawei Wang
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
 // TODO: add some commands

{
        .name      = "mba_add_systrace",
        .args_type = "label:s,cr3:l,sycall_num:i",
        .params    = "label cr3 sycall_num",
        .help      = "Trace `sycall_num` system call for process with `cr3`",
        .mhandler.cmd = do_add_systrace,
},
{
        .name      = "mba_delete_systrace",
        .args_type = "id:i",
        .params    = "id",
        .help      = "Delete the syscall tracer with given id",
        .mhandler.cmd = do_delete_systrace,
},

