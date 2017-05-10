/*
 *  mba sleuth kit extension of qemu command specification
 *
 *  Copyright (c)   2012 Chiwei Wang
 *                  2016 Chiawei Wang
 *                  2016 Chongkuan Chen
 *                  2016 Hao Li
 *                  2016 ChuanHua Cheng 
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
        .name = "mba_get_filename_by_haddr",
        .args_type  = "dev:B,addr:l",
        .params     = "dev addr",
        .help       = "Get the filename by image byte offset",
        .mhandler.cmd = do_get_filename_by_haddr,
},
{
        .name = "mba_get_haddrs_by_filename",
        .args_type  = "dev:B,filename:s",
        .params     = "dev filename",
        .help       = "Get the image byte addrs by filename",
        .mhandler.cmd = do_get_haddr_by_filename,
},{
        .name = "mba_get_file",
        .args_type  = "dev:B,filename:s,destination:F",
        .params     = "dev filename destination",
        .help       = "Get the file from the image throught low level disk forensics",
        .mhandler.cmd = do_get_file,
},{
        .name = "mba_search_registry_by_key",
        .args_type  = "key:s",
        .params     = "key",
        .help       = "Print registry by key",
        .mhandler.cmd = do_search_registry_by_key,
},{
        .name = "mba_print_registry_by_path",
        .args_type  = "path:s",
        .params     = "path",
        .help       = "Print registry by path",
        .mhandler.cmd = do_print_registry_by_path,
},{
        .name = "mba_print_registry_by_address",
        .args_type  = "address:s",
        .params     = "address",
        .help       = "Print registry by address",
        .mhandler.cmd = do_print_registry_by_address,
},{
        .name = "mba_get_part_file",
        .args_type  = "dev:B,filename:s,destination:F,offset:s,len:s",
        .params     = "dev filename destination offset len",
        .help       = "Get the part file from the image throught low level disk forensics",
        .mhandler.cmd = do_get_part_file,
},{
        .name = "mba_print_log",
        .args_type  = "dev:B,filename:s,destination:F,offset:s,len:s",
        .params     = "dev filename destination offset len",
        .help       = "Get the part file from the image throught low level disk forensics",
        .mhandler.cmd = do_get_part_file,
},
