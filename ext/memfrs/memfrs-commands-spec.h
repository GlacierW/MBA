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
        .name = "mba_load_structures",
        .args_type  = "sdb:F",
        .params     = "sdb",
        .help       = "loading structures information",
        .mhandler.cmd = do_load_structures,
},
{
        .name = "mba_list_processes",
        .args_type  = "",
        .params     = "",
        .help       = "listing running processes",
        .mhandler.cmd = do_process_list,
},
{
        .name = "mba_show_virmem_content",
        .args_type  = "addr:l,len:l,cr3:l?",
        .params     = "addr len [cr3]",
        .help       = "Show the virtual mem content starting at <addr> with length <len>",
        .mhandler.cmd = do_show_virmem_content,
},
{

        .name = "mba_show_phymem_content",
        .args_type  = "addr:l,len:l",
        .params     = "addr len",
        .help       = "Show the physical mem content starting at <addr> with length <len>",
        .mhandler.cmd = do_show_phymem_content,
},
{
        .name = "mba_get_physic_address",
        .args_type  = "cr3:l,addr:l",
        .params     = "cr3 addr",
        .help       = "get the physic address of a given virtual address in memory space(cr3)",
        .mhandler.cmd = do_get_physic_address,
},
{
        .name = "mba_scan_virmem_pattern",
        .args_type  = "start:l,end:l,pattern:s",
        .params     = "start end pattern",
        .help       = "Scan the virmem for the specific pattern",
        .mhandler.cmd = do_scan_virmem,
},
{
        .name = "mba_scan_phymem_pattern",
        .args_type  = "start:l,end:l,pattern:s",
        .params     = "start end pattern",
        .help       = "Scan the phymem for the specific pattern",
        .mhandler.cmd = do_scan_phymem,
},
{
        .name = "mba_travers_vad",
        .args_type  = "eprocess_addr:l",
        .params     = "eprocess_addr",
        .help       = "Trverse VAD tree",
        .mhandler.cmd = do_traverse_vad,
},
{
        .name = "mba_scan_kernel",
        .args_type  = "",
        .params     = "",
        .help       = "scan for the kernel",
        .mhandler.cmd = do_scan_kernel,
},
{
        .name = "mba_scan_module",
        .args_type  = "",
        .params     = "",
        .help       = "scan for the kernel module",
        .mhandler.cmd = do_module_list,
},
{
        .name = "mba_scan_network",
        .args_type  = "",
        .params     = "",
        .help       = "scan for the network",
        .mhandler.cmd = do_network_list,
},
{
        .name = "mba_load_global_variable",
        .args_type  = "gvar_db:F",
        .params     = "gvar_db",
        .help       = "loading global variable information",
        .mhandler.cmd = do_load_global_variable,
},
{
        .name = "mba_get_gvar_vmem",
        .args_type  = "gvar:s",
        .params     = "gvar",
        .help       = "get the virtual memory address of the global variable/symbol",
        .mhandler.cmd = do_get_gvar_vmem,
},
{
        .name = "mba_gvar_lookup",
        .args_type  = "addr:l",
        .params     = "addr",
        .help       = "get the symbolname at specific virtual memory address",
        .mhandler.cmd = do_gvar_lookup,
},
{
        .name = "mba_gen_pdb_profiles",
        .args_type  = "profiles_dir:F",
        .params     = "profiles_dir",
        .help       = "Generate PDB's global var and structure info into profiles_dir",
        .mhandler.cmd = do_gen_pdb_profiles,
},
{
        .name = "mba_dt",
        .args_type  = "addr:l,struct:s",
        .params     = "addr struct",
        .help       = "Fit the memory at addr into structure fields",
        .mhandler.cmd = do_display_type,
},
{
        .name = "mba_list_handles",
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
        .name = "mba_list_handles_types",
        .args_type  = "",
        .params     = "",
        .help       = "listing all types of handles",
        .mhandler.cmd = do_handles_types_list,
},
{
        .name = "mba_list_ssdt",
        .args_type  = "",
        .params     = "",
        .help       = "listing all system call name and address.",
        .mhandler.cmd = do_ssdt_list,
},
