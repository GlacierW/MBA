/*
 *  MBA agent extension of QEMU command implementation
 *
 *  Copyright (c)   2016 Chiawei Wang
 *                  2016 Jui-Chien, Jao
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

#include "agent-commands.h"
#include "agent.h"

void do_win_impo( Monitor *mon, const QDict *qdict )  {

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

void do_win_expo( Monitor *mon, const QDict *qdict )  {

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

void do_win_exec( Monitor *mon, const QDict *qdict ) {
   
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

void do_win_invo( Monitor *mon, const QDict *qdict ) {
    
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

void do_win_logf( Monitor *mon, const QDict *qdict ) {
    
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

void do_win_init( Monitor *mon, const QDict *qdict ) {

    MBA_AGENT_RETURN ret;

    // initialize agent
    // 0 tells the agent_init to new a fwd_port and send the function pointer 
    ret = agent_init( mon, 0, &net_slirp_redir );
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

}

void do_win_sync( Monitor *mon, const QDict *qdict ) {

    MBA_AGENT_RETURN ret;

    ret = agent_sync( );
    switch( ret ) { 

        case AGENT_RET_SUCCESS:
            monitor_printf( mon, "Agent Sync command is sent successfully, please wait for the ack-message\n" );
            break;

        case AGENT_RET_EBUSY:
            monitor_printf( mon, "Agent is busy handling the previous command. Come back later\n" );
            break;

        case AGENT_RET_EFAIL:
            monitor_printf( mon, "Agent Sync failed\n" );
            break;

        default:
            monitor_printf( mon, "Unkown error when reseting agent\n" );
            break;
    }
}

void do_win_reset( Monitor *mon, const QDict *qdict ) {

    MBA_AGENT_RETURN ret;
    
    ret = agent_reset( mon );
    switch( ret ) { 

        case AGENT_RET_SUCCESS:
            monitor_printf( mon, "Agent thread has been reset\n" );
            break;

        case AGENT_RET_EBUSY:
            monitor_printf( mon, "Agent is busy handling the previous command. Come back later\n" );
            break;

        case AGENT_RET_EFAIL:
            monitor_printf( mon, "Agent thread reset failed\n" );
            break;

        default:
            monitor_printf( mon, "Unkown error when reseting agent\n" );
            break;
    }
}
