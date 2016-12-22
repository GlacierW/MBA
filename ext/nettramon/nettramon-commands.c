/*
 *  MBA agent extension of QEMU command implementation
 *
 *  Copyright (c)    2016 Jui-Chien, Jao
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
        case 1 :
            monitor_printf( mon, " NetTraMon Started Successfully\n");
            break;
        case 0 :
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
        case 1 :
            monitor_printf( mon, " NetTraMon Stoped Successfully\n");
            break;
        case 0 :
            monitor_printf( mon, " NetTraMon Stoped Failed\n");
            break;
        default :
            return; 
    }

}
/*
void do_parse_packet ( Monitor *mon, const QDict *qdict )  {
    
    int retVal;

    retVal = nettramon_parse_buffer();

    switch( retVal ) {
        
        case 0 :
            monitor_printf( mon, buf );
            monitor_printf( mon, '\n' );
            break;
        case 2 :
            monitor_printf( mon, "Not Found Network Device\n" );
            break;
    }

}
*/
