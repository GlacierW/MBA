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
        .name = "load_structures",
        .args_type  = "sdb:s",
        .params     = "sdb",
        .help       = "loading structures information",
        .mhandler.cmd = do_load_structures,
},
{
        .name = "list_processes",
        .args_type  = "",
        .params     = "",
        .help       = "listing running processes",
        .mhandler.cmd = do_process_list,
},
{
        .name = "show_virmem_content",
        .args_type  = "addr:l,len:l,cr3:l?",
        .params     = "addr len [cr3]",
        .help       = "Show the virtual mem content starting at <addr> with length <len>",
        .mhandler.cmd = do_show_virmem_content,
},
{
        .name = "show_phymem_content",
        .args_type  = "addr:l,len:l",
        .params     = "addr len",
        .help       = "Show the physical mem content starting at <addr> with length <len>",
        .mhandler.cmd = do_show_phymem_content,
},
{
        .name = "scan_virmem_pattern",
        .args_type  = "start:l,end:l,pattern:s",
        .params     = "start end pattern",
        .help       = "Scan the virmem for the specific pattern",
        .mhandler.cmd = do_scan_virmem,
},
{
        .name = "scan_phymem_pattern",
        .args_type  = "start:l,end:l,pattern:s",
        .params     = "start end pattern",
        .help       = "Scan the phymem for the specific pattern",
        .mhandler.cmd = do_scan_phymem,
},
{
        .name = "travers_vad",
        .args_type  = "eprocess_addr:l",
        .params     = "eprocess_addr",
        .help       = "Trverse VAD tree",
        .mhandler.cmd = do_traverse_vad,
},
{
        .name = "scan_kernel",
        .args_type  = "",
        .params     = "",
        .help       = "scan for the kernel",
        .mhandler.cmd = do_scan_kernel,
},
{
        .name = "scan_module",
        .args_type  = "",
        .params     = "",
        .help       = "scan for the kernel module",
        .mhandler.cmd = do_module_list,
},
{
        .name = "load_global_variable",
        .args_type  = "gvar_db:s",
        .params     = "gvar_db",
        .help       = "loading global variable information",
        .mhandler.cmd = do_load_global_variable,
},
{
        .name = "get_gvar_vmem",
        .args_type  = "gvar:s",
        .params     = "gvar",
        .help       = "get the virtual memory address of the global variable/symbol",
        .mhandler.cmd = do_get_gvar_vmem,
},
{
        .name = "gvar_lookup",
        .args_type  = "addr:i",
        .params     = "addr",
        .help       = "get the symbolname at specific virtual memory address",
        .mhandler.cmd = do_gvar_lookup,
},
{
        .name = "list_handles",
        .args_type  = "target_type:s?,target:s?",
        .params     = "[-c cr3 | -e eprocess | -t handles_type]",
        .help       = "listing process running handles.\n"
                       "-c cr3 : parse handles of process by process' cr3\n"
                       "-e eprocess : parse handles of process by process' eprocess\n"
                       "-p pid : parse handles of process by process' pid\n"
                       "-t handles_type : parse only handles of handles_type"
                       "-f full_detail : parse only exactly match full_detail with handles' detail\n"
                       "-d detail : parse section matched detail with handles' detail",
        .mhandler.cmd = do_process_handles_list,
},
{
        .name = "list_handles_types",
        .args_type  = "",
        .params     = "",
        .help       = "listing all types of handles",
        .mhandler.cmd = do_handles_types_list,
},
