<<<<<<< HEAD
#include "qemu-common.h"
#include "monitor/monitor.h"
=======
/*
 *  MBA agent extension of QEMU command implementation
 *
 *  Copyright (c)   2016 Chiawei Wang
 *                  2016 Jui-Chien, Jai
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
#include "qemu-common.h"
#include "monitor/monitor.h"
#include "net/slirp.h"
>>>>>>> 75eb4731ce6ee01bcc4155e74db166a385f7a5c0

#include "agent-commands.h"
#include "agent.h"

<<<<<<< HEAD
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
=======
void do_win_impo( Monitor *mon, const QDict *qdict ) 
{

    MBA_AGENT_RETURN ret;

    char* dst_path = (char *)qdict_get_str(qdict, "dstpath");
    char* src_path = (char *)qdict_get_str(qdict, "srcpath");

    ret = agent_import( dst_path, src_path );

    switch( ret ) {

        case AGENT_RET_SUCCESS:
            break;

        case AGENT_RET_EFAIL:
            monitor_printf( mon, "Failed to setup 'import' action\n" );
            break;

        case AGENT_RET_EBUSY:
            monitor_printf( mon, "Agent is busy handling the previous command, Come back later\n" );
            break;

        case AGENT_RET_EINIT:
            monitor_printf( mon, "Please enter w_init to initialize agent thread\n" );
            break;

        default:
            monitor_printf( mon, "Unknown failure of import. Code: %d\n", ret );
            break;
    }
}

void do_win_expo( Monitor *mon, const QDict *qdict ) 
{

    MBA_AGENT_RETURN ret;

    char* dst_path = (char *)qdict_get_str(qdict, "dstpath");
    char* src_path = (char *)qdict_get_str(qdict, "srcpath");

    ret = agent_export( dst_path, src_path );

    switch( ret ) {

        case AGENT_RET_SUCCESS:
            break;

        case AGENT_RET_EFAIL:
            monitor_printf( mon, "Failed to setup 'export' action\n" );
            break;

        case AGENT_RET_EBUSY:
            monitor_printf( mon, "Agent is busy handling the previous command. Come back later\n" );
            break;

        case AGENT_RET_EINIT:
            monitor_printf( mon, "Please enter w_init to initialize agent thread\n" );
            break;

        default:
            monitor_printf( mon, "Unknown failure of export. Code: %d\n", ret );
            break;
    }
}

void do_win_exec( Monitor *mon, const QDict *qdict )
{
    MBA_AGENT_RETURN ret;

    char* cmdline = (char*)qdict_get_str(qdict, "cmdline");

    ret = agent_execute( cmdline );

    switch( ret ) { 

        case AGENT_RET_SUCCESS:
            break;

        case AGENT_RET_EFAIL:
            monitor_printf( mon, "Failed to setup 'execute' action\n" );
            break;

        case AGENT_RET_EBUSY:
            monitor_printf( mon, "Agent is busy handling the previous command. Come back later\n" );
            break;

        case AGENT_RET_EINIT:
            monitor_printf( mon, "Please enter w_init to initialize agent thread\n" );
            break;

        default:
            monitor_printf( mon, "Unknown failure of execute. Code: %d\n", ret );
            break;
    }   
}

void do_win_invo( Monitor *mon, const QDict *qdict )
{
    MBA_AGENT_RETURN ret;

    char* cmdline = (char*)qdict_get_str(qdict, "cmdline");

    ret = agent_invoke( cmdline );

    switch( ret ) {

        case AGENT_RET_SUCCESS:
            break;

        case AGENT_RET_EFAIL:
            monitor_printf( mon, "Failed to setup 'invoke' action\n" );
            break;

        case AGENT_RET_EBUSY:
            monitor_printf( mon, "Agent is busy handling the previous command. Come back later\n" );
            break;

        case AGENT_RET_EINIT:
            monitor_printf( mon, "Please enter w_init to initialize agent thread\n" );
            break;

        default:
            monitor_printf( mon, "Unknown failure of invoke. Code: %d\n", ret );
            break;
    }

}

void do_win_logf( Monitor *mon, const QDict *qdict )
{
    MBA_AGENT_RETURN ret;

    char* dst_path = (char*)qdict_get_str(qdict, "dstpath");

    ret = agent_logfile( dst_path );

    switch( ret ) { 

        case AGENT_RET_SUCCESS:
            break;

        case AGENT_RET_EFAIL:
            monitor_printf( mon, "Failed to setup 'log file export' action\n" );
            break;

        case AGENT_RET_EBUSY:
            monitor_printf( mon, "Agent is busy handling the previous command, Come back later\n" );
            break;

        case AGENT_RET_EINIT:
            monitor_printf( mon, "Please enter w_init to initialize agent thread\n" );
            break;

        default:
            monitor_printf( mon, "Unknown failure of log file export. Code: %d\n", ret );
            break;
    }   
}

void do_win_init( Monitor *mon, const QDict *qdict )
{
    MBA_AGENT_RETURN ret;

    // counter (down) for port forwarding setup try
    int try_countdown = 10; 

    uint16_t fwd_port;
    char     fwd_config[32];
        
    // check if the agent extension has been initialized
    if( agent_is_ready() ) { 
        monitor_printf( mon, "The agent has been initialized\n" );
        return;
    }   

    // setup port forwarding for in-VM agent server, 10 times trying with random port
    srand( time(NULL) );
    while( try_countdown ) { 

        fwd_port  = rand() % 65535;
        fwd_port += (fwd_port < 1024 )? 1024 : 0;

        bzero( fwd_config, sizeof(fwd_config) );
        snprintf( fwd_config, 32, "tcp:%d::%d", fwd_port, AGENT_GUEST_PORT );

        if( net_slirp_redir(fwd_config) == 0 ) 
            break;

        --try_countdown;
    }   

    if( try_countdown == 0 ) { 
        monitor_printf( mon, "Agent port forwarding setup failed\n" );
        return;
    }   

    // initialize agent
    ret = agent_init( mon, fwd_port );
    switch( ret ) { 

        case AGENT_RET_SUCCESS:
            monitor_printf( mon, "Agent thread starting\n" );
            break;

        case AGENT_RET_EBUSY:
            monitor_printf( mon, "Agent is busy handling the previous command. Come back later\n" );
            break;

        case AGENT_RET_EFAIL:
            monitor_printf( mon, "Agent thread start failed\n" );
            break;

        default:
            monitor_printf( mon, "Unkown error when initializing agent\n" );
            break;
    }   

>>>>>>> 75eb4731ce6ee01bcc4155e74db166a385f7a5c0
}

