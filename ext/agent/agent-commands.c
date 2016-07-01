#include "qemu-common.h"
#include "monitor/monitor.h"
#include "net/slirp.h"

#include "agent-commands.h"
#include "agent.h"

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

}

