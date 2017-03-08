/*
 *  MBA agent extension of QEMU command specification
 *
 *  Copyright (c)   2016 Chiawei Wang
 *                  2016 Jui-Chien, Jao
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
    .name         = "mba_wimpo",
    .args_type    = "srcpath:F,dstpath:s",
    .params       = "fullpath_host_srcfile fullpath_guest_dstfile",
    .help         = "Import a host file into the guest."
                    "\n\t\t\tFullpath without spaces of both the source file and the destination file are required.",
    .mhandler.cmd = do_win_impo,
},
{
    .name         = "mba_wexpo",
    .args_type    = "srcpath:s,dstpath:F",
    .params       = "fullpath_guest_srcfile fullpath_host_dstfile",
    .help         = "Export a guest file to the host."
                    "\n\t\t\tFullpath without spaces of both the source file and the destination file are required.",
    .mhandler.cmd = do_win_expo,
},
{
    .name         = "mba_wexec",
    .args_type    = "cmdline:s",
    .params       = "command_to_execute",
    .help         = "Execute the given command inside the guest and perform interactive Input/Output.",
    .mhandler.cmd = do_win_exec,
},
{
    .name         = "mba_winvo",
    .args_type    = "cmdline:s",
    .params       = "command_to_execute",
    .help         = "Invoke the given command inside the guest without waiting the result/output.",
    .mhandler.cmd = do_win_invo,
},
{
    .name         = "mba_wlogf",
    .args_type    = "dstpath:s",
    .params       = "fullpath_logfile",
    .help         = "Export the agent log file from the guest to the host. The fullpath of the destination file is required.",
    .mhandler.cmd = do_win_logf,
},
{
    .name         = "mba_winit",
    .args_type    = "",
    .params       = "",
    .help         = "Create an agent thread to communicate with Windows in-VM agent server.",
    .mhandler.cmd = do_win_init,
},
{
    .name         = "mba_wsync",
    .args_type    = "", 
    .params       = "",
    .help         = "Flush the Guest OS cache.",
    .mhandler.cmd = do_win_sync,
},
{
    .name         = "mba_wrset",
    .args_type    = "", 
    .params       = "",
    .help         = "Reset the agent client to the condition before w_init.",
    .mhandler.cmd = do_win_reset,
},
