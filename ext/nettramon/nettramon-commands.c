/*
 *  MBA agent extension of QEMU command implementation
 *
 *  Copyright (c)    2017 Jui-Chien, Jao
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

#include "nettramon-commands.h"
#include "nettramon.h"

void do_nettramon_start ( Monitor *mon, const QDict *qdict ) {

    int retVal;

    retVal = nettramon_start( mon );

    switch( retVal ) {
        case 0 :
            monitor_printf( mon, " NetTraMon Started Successfully\n");
            break;
        case 1 :
            monitor_printf( mon, " Written files are set, no printing out on monitor\n" );
            break;
        case 2 :
            monitor_printf( mon, " NetTraMon Started Failed \n");
            break;
        default :
            return; 
    }

}

void do_nettramon_stop ( Monitor *mon, const QDict *qdict ) {

    int retVal;

    retVal = nettramon_stop( );

    switch( retVal ) {
        case 0 :
            monitor_printf( mon, " NetTraMon Stopped Successfully\n");
            break;
        case 1 :
            monitor_printf( mon, " NetTraMon Stopped Failed\n");
            break;
        default :
            return; 
    }

}

void do_nettramon_set_file_path ( Monitor *mon, const QDict *qdict )  {
    
    int retVal;
    char* all_file_path  = NULL;
    char* tcp_file_path  = NULL;
    char* udp_file_path  = NULL;
    char* icmp_file_path = NULL;

    all_file_path  = (char *)qdict_get_str(qdict, "all_file");
    tcp_file_path  = (char *)qdict_get_str(qdict, "tcp_file");
    udp_file_path  = (char *)qdict_get_str(qdict, "udp_file");
    icmp_file_path = (char *)qdict_get_str(qdict, "icmp_file");

    retVal = nettramon_set_file_path( all_file_path, tcp_file_path, udp_file_path, icmp_file_path );

    switch( retVal ) {
        
        case 0 :
            monitor_printf( mon, "Set written files successfully\n" );
            break;
        case 1 :
            monitor_printf( mon, "Failed to set written files\n" );
            break;
    }

}

void do_nettramon_reset_file_path ( Monitor *mon, const QDict *qdict )  {
    
    int retVal;

    retVal = nettramon_reset_file_path( );

    switch( retVal ) {
        
        case 0 :
            monitor_printf( mon, "Reset written files successfully\n" );
            break;
        case 1 :
            monitor_printf( mon, "Failed to reset written files\n" );
            break;
    }

}

void do_nettramon_set_filter ( Monitor *mon, const QDict *qdict )  {
    
    int retVal;
    char* filter_string = NULL;
    
    filter_string  = (char *)qdict_get_str(qdict, "filter_string");

    retVal = nettramon_set_filter( filter_string );

    switch( retVal ) {
        
        case 0 :
            monitor_printf( mon, "Set packet filter successfully\n" );
            break;
        case 1 :
            monitor_printf( mon, "Failed to set packet filter\n" );
            break;
    }

}

void do_nettramon_reset_filter ( Monitor *mon, const QDict *qdict )  {
    
    int retVal;
    
    retVal = nettramon_reset_filter(  );

    switch( retVal ) {
        
        case 0 :
            monitor_printf( mon, "Reset packet filter successfully\n" );
            break;
        case 1 :
            monitor_printf( mon, "Failed to reset pcaket filter\n" );
            break;
    }

}
