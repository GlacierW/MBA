/*
 *  mba sleuth kit extension of qemu command specification
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
        .name = "get_filename_by_haddr",
        .args_type  = "dev:s,addr:l",
        .params     = "dev addr",
        .help       = "Get the filename by image byte offset",
        .mhandler.cmd = do_get_filename_by_haddr,
},
{
        .name = "get_haddrs_by_filename",
        .args_type  = "dev:s,filename:s",
        .params     = "dev filename",
        .help       = "Get the image byte addrs by filename",
        .mhandler.cmd = do_get_haddr_by_filename,
},
