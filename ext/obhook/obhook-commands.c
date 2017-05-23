/*
 *  HMP command implementation of the MBA out-of-box hook
 *
 *  Copyright (c)   2016 Chiawei Wang
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
#include "qmp-commands.h"
#include "obhook-commands.h"
#include "obhook.h"

#include "sysemu/sysemu.h"
#include "qapi-types.h"

void do_list_obhook( Monitor* mon, const QDict* qdict ) {

    obhk_ht_record* ht_rec;
    obhk_ht_record* ht_tmp;

    obhk_ht_record* ht_proc_rec;
    obhk_ht_record* ht_proc_tmp;

    obhk_cb_record* cb_rec;

    HASH_ITER( hh, obhk_ctx->hook_tbl, ht_rec, ht_tmp ) {

        if( ht_rec == NULL )
            break;

        if( ht_rec->cr3 == 0 )
            monitor_printf( mon, "----- Universal\n" );
        else
            monitor_printf( mon, "----- Process %016lx\n", ht_rec->cr3 );

        HASH_ITER( hh, ht_rec->proc_obhk_tbl, ht_proc_rec, ht_proc_tmp ) {

            if( ht_proc_rec == NULL )
                break;

            monitor_printf( mon, "    %016lx: \n", ht_proc_rec->addr );
            LL_FOREACH( ht_proc_rec->cb_list, cb_rec ) {
                monitor_printf( mon, "\t%5d, %10s, %16s\n", 
                            cb_rec->uniq_d,
                            (cb_rec->enabled)? "enabled" : "disabled",
                            cb_rec->label );
            }
        }
    }
    printf( "\n" );
}

void do_delete_obhook( Monitor* mon, const QDict* qdict ) {

    int obhook_d = qdict_get_int( qdict, "obhook_d" );

    if( obhook_delete(obhook_d) != 0 ) 
        monitor_printf( mon, "fail to delete the hook\n" );
    else
        monitor_printf( mon, "success\n" );
}

void do_enable_obhook( Monitor* mon, const QDict* qdict ) {
    
    int obhook_d = qdict_get_int( qdict, "obhook_d" );

    if( obhook_enable(obhook_d) != 0 ) 
        monitor_printf( mon, "fail to enable the hook\n" );
    else
        monitor_printf( mon, "success\n" );
}

void do_disable_obhook( Monitor* mon, const QDict* qdict ) {
    
    int obhook_d = qdict_get_int( qdict, "obhook_d" );

    if( obhook_disable(obhook_d) != 0 ) 
        monitor_printf( mon, "fail to disable the hook\n" );
    else
        monitor_printf( mon, "success\n" );
}
