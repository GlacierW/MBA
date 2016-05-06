#include "qemu-common.h"
#include "monitor/monitor.h"

#include "agent-commands.h"
#include "agent.h"

void do_win_impo(Monitor *mon, const QDict *qdict)
{
	char *srcpath = (char *)qdict_get_str(qdict, "srcpath");
	char *despath = (char *)qdict_get_str(qdict, "despath");
	unsigned int file_size = (uint32_t)qdict_get_int(qdict, "size");
	
	if ( file_size > 100*1024*1024 )
	{
		monitor_printf(mon, "Impo file size should be less than 100MB.\n");
	}
	else
	{
		strcpy(agent_srcpath, srcpath);
		strcpy(agent_despath, despath);
		agent_file_size = file_size;
		agent_action = 1;
	}
}
void do_win_expo(Monitor *mon, const QDict *qdict)
{
    	char *srcpath = (char *)qdict_get_str(qdict, "srcpath");
    	char *despath = (char *)qdict_get_str(qdict, "despath");
	unsigned int file_size = (uint32_t)qdict_get_int(qdict, "size");
	
	if ( file_size > 100*1024*1024 )
	{
		monitor_printf(mon, "Expo file size should be less than 100MB.\n");
	}
	else
	{
		strcpy(agent_srcpath, srcpath);
                strcpy(agent_despath, despath);
                agent_file_size = file_size;

		agent_action = 2;
	}
}
void do_win_exec(Monitor *mon, const QDict *qdict)
{
    	char *despath = (char *)qdict_get_str(qdict, "despath");
	unsigned int file_size = 0;

	agent_srcpath = NULL;
        strcpy(agent_despath, despath);
        agent_file_size = file_size;

	agent_action = 3;
}
void do_win_invo(Monitor *mon, const QDict *qdict)
{
	char *despath = (char *)qdict_get_str(qdict, "despath");
	unsigned int file_size = 0;

	agent_srcpath = NULL;
        strcpy(agent_despath, despath);
        agent_file_size = file_size;

	agent_action = 4;
}
void do_win_init(Monitor *mon, const QDict *qdict)
{
	agent_srcpath = NULL;
	agent_despath = NULL;
	agent_file_size = 0;
	create_agent_thread(mon);
}
void do_win_stat(Monitor *mon, const QDict *qdict)
{
	agent_srcpath = NULL;
        agent_despath = NULL;
	agent_file_size = 0;
	agent_action = 5;
}

