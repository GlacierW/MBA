/*
 *  mba sleuth kit extension of qemu command header
 *
 *  Copyright (c)   2012 Chiwei Wang
 *                  2016 Chiawei Wang
 *                  2016 Chongkuan Chen
 *                  2016 Hao Li
 *                  2016 ChuanHua Cheng
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
#ifndef __TSK_COMMAND_H_
#define __TSK_COMMAND_H_
#define CHECK_REGISTRY_PATH 2

struct Monitor;
struct QDict;
void do_get_filename_by_haddr(Monitor *mon, const QDict *qdict);
void do_get_haddr_by_filename(Monitor *mon, const QDict *qdict);
void do_get_file(Monitor *mon, const QDict *qdict);
void do_get_part_file(Monitor *mon, const QDict *qdict);
void download_hive_to_tmp(Monitor *mon);
void do_search_registry_by_key(Monitor *mon, const QDict *qdict);
int search_registry_by_key(const char* key);
void do_print_registry_by_path(Monitor *mon, const QDict *qdict);
int print_registry_by_path(const char* path);
void pathHandle(const char* path);
void do_print_registry_by_address(Monitor *mon, const QDict *qdict);


#endif
