#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <monitor/monitor.h>

#include "agent.h"

void* agent_thread_main(void* monitor);
char* agent_srcpath = NULL;
char* agent_despath = NULL;
unsigned int agent_action = 0;
unsigned int agent_file_size = 0;

void* agent_thread_main(void *monitor)
{
	Monitor* mon = (Monitor* )monitor;
	monitor_printf(mon, "Agent Thread Starting.\n");

        while(1)
        {
                while ( agent_action == 0 );
                if ( agent_action == 1 )
                {
                        monitor_printf(mon, "get impo cmd From:%s To:%s Size:%d\n", agent_srcpath, agent_despath, agent_file_size);
                }
                else if ( agent_action == 2 )
                {
                        monitor_printf(mon, "get expo cmd From:%s To:%s Size:%d\n", agent_srcpath, agent_despath, agent_file_size);
                }
                else if ( agent_action == 3 )
                {
                        monitor_printf(mon, "get exec cmd %s\n", agent_despath);
                }
                else if ( agent_action == 4 )
                {
                        monitor_printf(mon, "get invo cmd %s\n", agent_despath);
                }
		else if ( agent_action == 5 )
		{
			monitor_printf(mon, "get status cmd\n");	
		}
		else{}
		agent_action = 0;
		sleep(1);
        }
}

void create_agent_thread(Monitor *mon)
{
	pthread_t agent_thread;
	pthread_create(&agent_thread, NULL, agent_thread_main, (void *)mon);
}
