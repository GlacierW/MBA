/*
 *  MBA agent extension of QEMU command specification
 *
 *  Copyright (c)    2017 Jui-Chien, Jao
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
    .name           = "ntm_start",
    .args_type      = "",
    .params         = "",
    .help           = "Turn on Network Traffic Monitor, and print packet's content on screen or into file if the filepaths have been set",
    .mhandler.cmd   = do_nettramon_start,
},
{
    .name           = "ntm_stop",
    .args_type      = "",
    .params         = "",
    .help           = "Turn off Network Traffic Monitor, but not clean the settings of filepaths and the filter",
    .mhandler.cmd   = do_nettramon_stop,
},
{
    .name           = "ntm_set_file_path",
    .args_type      = "all_file:F,tcp_file:F,udp_file:F,icmp_file:F",
    .params         = "all_file tcp_file udp_file icmp_file",
    .help           = "Set the files for writing captured packets' information according to their types " 
                      "and there must be one strings for each type of packet. *** 'N' for no printing ***",
    .mhandler.cmd   = do_nettramon_set_file_path,
},
{
    .name           = "ntm_reset_file_path",
    .args_type      = "",
    .params         = "",
    .help           = "Reset the filepaths for writing captured packet information",
    .mhandler.cmd   = do_nettramon_reset_file_path,
},
{
    .name           = "ntm_set_filter",
    .args_type      = "filter_string:s",
    .params         = "filter_string",
    .help           = "Set the filter string",
    .mhandler.cmd   = do_nettramon_set_filter,
},
{
    .name           = "ntm_reset_filter",
    .args_type      = "",
    .params         = "",
    .help           = "Reset the filter string",
    .mhandler.cmd   = do_nettramon_reset_filter,
},
