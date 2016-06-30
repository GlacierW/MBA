/*
{
        .name         = "w_impo",
        .args_type    = "srcpath:s,despath:s",
        .params       = "srcpath despath",
        .help         = "Impo commands from QEMU to Windows agent. Need the size in bytes of target flie, full path of target file and full destination path in Windows.",
        .mhandler.cmd = do_win_impo,
},
{
	.name         = "w_expo",
        .args_type    = "srcpath:s,despath:s",
        .params       = "srcpath despath",
        .help         = "Expo commands from QEMU to Windows agent. Need the size in bytes of target flie, full destination path and full path of target file in Windows.",
        .mhandler.cmd = do_win_expo,
},
{
	.name         = "w_exec",
        .args_type    = "despath:s",
        .params       = "despath",
        .help         = "Exec commands from QEMU to Windows agent. Need full path of target file in Windows.",
        .mhandler.cmd = do_win_exec,
},
{
	.name	      = "w_invo",
        .args_type    = "despath:s",
        .params       = "despath",
        .help         = "Invo commands from QEMU to Windows agent. Need full path of target file in Windows.",
        .mhandler.cmd = do_win_invo,
},
{
	.name	      = "w_stat",
	.args_type    = "",
	.params       = "",
	.help         = "Check Windows agent's status.",
	.mhandler.cmd = do_win_status,
},
{
	.name	      = "w_logf",
	.args_type    = "despath:s",
	.params       = "despath",
	.help         = "Get log's file",
	.mhandler.cmd = do_win_logf,
},
*/
{
	.name  	      = "w_init",
	.args_type    = "",
	.params       = "",
	.help	      = "Create a thread to communicate with Windows agent.",
	.mhandler.cmd = do_win_init,
},

