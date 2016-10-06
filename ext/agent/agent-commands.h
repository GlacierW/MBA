/*
 *  MBA agent extension of QEMU command header
 *
 *  Copyright (c)   2016 Chiawei Wang
 *                  2016 Jui-Chien, Jao
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
#ifndef __AGENT_COMMANDS_H__
#define __AGENT_COMMANDS_H__

struct Monitor;
struct QDict;
void do_win_impo(struct Monitor *mon, const struct QDict *qdict);
void do_win_expo(struct Monitor *mon, const struct QDict *qdict);
void do_win_exec(struct Monitor *mon, const struct QDict *qdict);
void do_win_invo(struct Monitor *mon, const struct QDict *qdict);
void do_win_logf(struct Monitor *mon, const struct QDict *qdict);
void do_win_init(struct Monitor *mon, const struct QDict *qdict);
#endif
