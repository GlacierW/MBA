/*
 *  MBA Dynamic Behavior Analyzer QEMU command specification
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
{
        .name         = "start_dba_task",
        .args_type    = "sample:F,timer:i",
        .params       = "sample timer",
        .help         = "Start dynamic behavior analysis(DBA) on the given sample",
        .mhandler.cmd = do_start_dba_task,
},
