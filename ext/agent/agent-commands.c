#include "qemu-common.h"
#include "monitor/monitor.h"

#include "agent-commands.h"
#include "agent.h"

void do_win_impo(Monitor *mon, const QDict *qdict)
{
    if ( agent_thread == 1 ) {
        char *srcpath = (char *)qdict_get_str(qdict, "srcpath");
        char *despath = (char *)qdict_get_str(qdict, "despath");
        strcpy(agent_srcpath, srcpath);
        strcpy(agent_despath, despath);
        agent_action = 1;
    } // if
    else {
        monitor_printf(mon, "Please enter w_init to start thread\n");
    } // else
}
void do_win_expo(Monitor *mon, const QDict *qdict)
{
    if ( agent_thread == 1 ) {
	    char *srcpath = (char *)qdict_get_str(qdict, "srcpath");
	    char *despath = (char *)qdict_get_str(qdict, "despath");
        strcpy(agent_srcpath, srcpath);
        strcpy(agent_despath, despath);
        agent_action = 2;
    } // if
    else {
        monitor_printf(mon, "Please enter w_init to start thread\n");
    } // else
}
void do_win_exec(Monitor *mon, const QDict *qdict)
{
    if ( agent_thread == 1 ) {
	    char *despath = (char *)qdict_get_str(qdict, "despath");
        strcpy(agent_despath, despath);
        agent_action = 3;
    } // if
    else {
        monitor_printf(mon, "Please enter w_init to start thread\n");
    } // else
}
void do_win_invo(Monitor *mon, const QDict *qdict)
{
    if ( agent_thread == 1 ) {
	    char *despath = (char *)qdict_get_str(qdict, "despath");
        strcpy(agent_despath, despath);
	    agent_action = 4;
    } // if
    else {
        monitor_printf(mon, "Please enter w_init to start thread\n");
    } // else
}
void do_win_init(Monitor *mon, const QDict *qdict)
{
    if ( agent_thread != 1 )
        agent_thread = 0;
	create_agent_thread(mon);
}
void do_win_status(Monitor *mon, const QDict *qdict)
{
    if ( agent_thread == 1 ) {
        agent_action = 5;
    } // if
    else {
        monitor_printf(mon, "Please enter w_init to start thread\n");
    } // else
}
void do_win_logf(Monitor *mon, const QDict *qdict)
{
     if ( agent_thread == 1 ) {
	    char *despath = (char *)qdict_get_str(qdict, "despath");
        strcpy(agent_despath, despath);
        agent_action = 6;
    } // if
    else {
        monitor_printf(mon, "Please enter w_init to start thread\n");
    } // else
}
