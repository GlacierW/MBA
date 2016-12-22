/*
 *  MBA agent extension of QEMU command header
 *
 *  Copyright (c)    2016 Jui-Chien, Jao
 *
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
#ifndef __NETTRAMON_COMMANDS_H__
#define __NETTRAMON_COMMANDS_H__

#define NET_DEVICE_NAME_LENGTH 64

void do_nettramon_start (struct Monitor *mon, const struct QDict *qdict);
void do_nettramon_stop  (struct Monitor *mon, const struct QDict *qdict);

#endif
