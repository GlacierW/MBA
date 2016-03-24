{
        .name = "get_filename_by_haddr",
        .args_type  = "dev:s,addr:l",
        .params     = "addr dev",
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
