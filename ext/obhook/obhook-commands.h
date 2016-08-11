/*
 *  HMP command header of the MBA out-of-box hook
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
#ifndef __OBHOOK_COMMANDS_H__
#define __OBHOOK_COMMANDS_H__

struct Monitor;
struct QDict;

void do_list_obhook( Monitor* mon, const QDict* qdict );
void do_delete_obhook( Monitor* mon, const QDict* qdict );
void do_enable_obhook( Monitor* mon, const QDict* qdict );
void do_disable_obhook( Monitor* mon, const QDict* qdict );

#endif
