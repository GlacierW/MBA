/*
 *  HMP command header of the MBA DIFT
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
#ifndef __DIFT_COMMANDS_H__
#define __DIFT_COMMANDS_H__

struct Monitor;
struct QDict;
void do_set_memory_taint(struct Monitor *mon, const struct QDict *qdict);
void do_show_memory_taint_map(struct Monitor *mon, const struct QDict *qdict);
void do_set_disk_taint(struct Monitor *mon, const struct QDict *qdict);
void do_show_disk_taint_map(struct Monitor *mon, const struct QDict *qdict);
void do_get_physic_address(struct Monitor *mon, const struct QDict *qdict);
void do_enable_dift(struct Monitor *mon, const struct QDict *qdict);
void do_disable_dift(struct Monitor *mon, const struct QDict *qdict);
void do_dift_status(struct Monitor *mon, const struct QDict *qdict);
#endif
