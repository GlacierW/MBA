/*
 *  MBA Virtual Machine Memory Forensic extension of qemu command header
 *
 *  Copyright (c)   2012 Chiwei Wang
 *                  2016 Chiawei Wang
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
#ifndef __MEMFRS_COMMAND_H_
#define __MEMFRS_COMMAND_H_

struct Monitor;
struct QDict;
void do_process_list(Monitor *mon, const QDict *qdict);
void do_load_structures(Monitor *mon, const QDict *qdict);
void do_show_virmem_content(Monitor *mon, const QDict *qdict);
void do_show_phymem_content(Monitor *mon, const QDict *qdict);
void do_get_physic_address(struct Monitor *mon, const struct QDict *qdict);
void do_scan_virmem(Monitor *mon, const QDict *qdict);
void do_scan_phymem(Monitor *mon, const QDict *qdict);
void do_traverse_vad(Monitor *mon, const QDict *qdict);
void do_scan_kernel(Monitor *mon, const QDict *qdict);
void do_get_windows_version(Monitor *mon, const QDict *qdict);
void do_module_list(Monitor *mon, const QDict *qdict);
void do_load_global_variable(Monitor *mon, const QDict *qdict);
void do_get_gvar_vmem(Monitor *mon, const QDict *qdict);
void do_gvar_lookup(Monitor *mon, const QDict *qdict);
void do_gen_pdb_profiles(Monitor *mon, const QDict *qdict);
void do_display_type(Monitor *mon, const QDict *qdict);

//void hexdump(Monitor *mon, uint8_t* buf, size_t length);
void do_process_handles_list(Monitor *mon, const QDict *qdict);
void do_handles_types_list(Monitor *mon, const QDict *qdict);
void do_network_list(Monitor *mon, const QDict *qdict);
void do_ssdt_list(Monitor *mon, const QDict *qdict);
#endif

