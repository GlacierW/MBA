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
        .name         = "mba_start_dba",
        .args_type    = "sample:F,timer:i,config:F?",
        .params       = "sample timer [config]",
        .help         = "Start dynamic behavior analysis(DBA) on the given sample",
        .mhandler.cmd = do_start_dba_task,
},
{
        .name         = "mba_list_dba_task",
        .args_type    = "tid:i?",
        .params       = "[task_id]",
        .help         = "List DBA tasks",
        .mhandler.cmd = do_list_dba_task,
},
{
        .name         = "mba_delete_dba_task",
        .args_type    = "tid:i",
        .params       = "task_id",
        .help         = "Delete a DBA task by ID",
        .mhandler.cmd = do_delete_dba_task,
},
{
        .name         = "mba_show_dba_result",
        .args_type    = "tid:i,file:F?",
        .params       = "task_id [file]",
        .help         = "Show the analysis result of the DBA task specified by ID." \
                        "Can specify a file in order to write into it.",
        .mhandler.cmd = do_show_dba_result,
},
