/*
 *  mba analysis extension of qemu command header
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
#ifndef __ANALYSIS_COMMAND_H_
#define __ANALYSIS_COMMAND_H_

#include "qemu-common.h"
struct Monitor;
struct QDict;
void do_get_analysis_result(Monitor *mon, const QDict *qdict);
void do_analyze_tainted_files(Monitor *mon, const QDict *qdict);
void do_scan_tainted_files(Monitor *mon, const QDict *qdict);
#endif
