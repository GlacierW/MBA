/*
 *  MBA agent extension of QEMU command specification
 *
 *  Copyright (c)    2016 Jui-Chien, Jao
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
    .help           = "Turn on Network Traffic Monitor",
    .mhandler.cmd   = do_nettramon_start,
},
{
    .name           = "ntm_stop",
    .args_type      = "",
    .params         = "",
    .help           = "Turn off Network Traffic Monitor",
    .mhandler.cmd   = do_nettramon_stop,
},
