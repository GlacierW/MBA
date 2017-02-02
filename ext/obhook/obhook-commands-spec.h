/*
 *  MBA Out-of-Box Hooking QEMU command specification 
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
        .name      = "mba_list_obhook",
        .args_type = "",
        .params    = "",
        .help      = "list all registered out-of-box hooks",
        .mhandler.cmd = do_list_obhook,
},
{
        .name      = "mba_delete_obhook",
        .args_type = "obhook_d:i",
        .params    = "obhook_descriptor",
        .help      = "delete an out-of-box hook specified by the descriptor number",
        .mhandler.cmd = do_delete_obhook,
},
{
        .name      = "mba_enable_obhook",
        .args_type = "obhook_d:i",
        .params    = "obhook_descriptor",
        .help      = "enable an out-of-box hook specified by the descriptor number",
        .mhandler.cmd = do_enable_obhook,
},
{
        .name      = "mba_disable_obhook",
        .args_type = "obhook_d:i",
        .params    = "obhook_descriptor",
        .help      = "disable an out-of-box hook specified by the descriptor number",
        .mhandler.cmd = do_disable_obhook,
},
