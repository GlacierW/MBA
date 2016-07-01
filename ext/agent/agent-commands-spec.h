{
    .name         = "w_impo",
    .args_type    = "srcpath:s,dstpath:s",
    .params       = "fullpath_host_srcfile fullpath_guest_dstfile",
    .help         = "Import a host file into the guest."
                    "\n\t\t\tFullpath without spaces of both the source file and the destination file are required.",
    .mhandler.cmd = do_win_impo,
},
{
    .name         = "w_expo",
    .args_type    = "srcpath:s,dstpath:s",
    .params       = "fullpath_guest_srcfile fullpath_host_dstfile",
    .help         = "Export a guest file to the host."
                    "\n\t\t\tFullpath without spaces of both the source file and the destination file are required.",
    .mhandler.cmd = do_win_expo,
},
{
	.name         = "w_exec",
    .args_type    = "cmdline:s",
    .params       = "command_to_execute",
    .help         = "Execute the given command inside the guest and perform interactive Input/Output.",
    .mhandler.cmd = do_win_exec,
},
{
    .name         = "w_invo",
    .args_type    = "cmdline:s",
    .params       = "command_to_execute",
    .help         = "Invoke the given command inside the guest without waiting the result/output.",
    .mhandler.cmd = do_win_invo,
},
{
	.name	      = "w_logf",
	.args_type    = "dstpath:s",
	.params       = "fullpath_logfile",
	.help         = "Export the agent log file from the guest to the host. The fullpath of the destination file is required.",
	.mhandler.cmd = do_win_logf,
},
{
	.name  	      = "w_init",
	.args_type    = "",
	.params       = "",
	.help	      = "Create an agent thread to communicate with Windows in-VM agent server.",
	.mhandler.cmd = do_win_init,
},
