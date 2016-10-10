/*
 *  mba analysis extension of qemu command specification
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
{
        .name = "analyze_tainted_files",
        .args_type  = "sample_path:s",
        .params     = "sample_path",
        .help       = "Analyze sample program and print report",
        .mhandler.cmd = do_analyze_tainted_files,
},
{
        .name = "get_analysis_result",
        .args_type  = "",
        .params     = "",
        .help       = "Get result of last analysis",
        .mhandler.cmd = do_get_analysis_result,
},
{
        .name = "scan_tainted_files",
        .args_type  = "",
        .params     = "",
        .help       = "scan disk to find tainted files",
        .mhandler.cmd = do_scan_tainted_files,
},
