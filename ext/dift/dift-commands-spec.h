{
        .name = "contaminate_mem",
        .args_type  = "addr:l,range:l,contaminate:l",
        .params     = "addr range contaminate",
        .help       = "contaminate a byte in memory",
        .mhandler.cmd = do_memory_contaminate,
},
{
        .name = "show_memory_taint_map",
        .args_type  = "addr:l,len:l",
        .params     = "addr len",
        .help       = "Show memory taint map",
        .mhandler.cmd = do_show_memory_taint_map,
},
