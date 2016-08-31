/*
 *  MBA helpers implementation, including
 *      Out-of-Box Hook
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

#include "cpu.h"
#include "exec/helper-proto.h"

#if defined(CONFIG_OBHOOK)
#include "../ext/obhook/obhook.h"
#endif

#if defined(CONFIG_OBHOOK)
void helper_obhook_dispatcher( CPUX86State* env ) {

    obhk_cb_record* cb_list = NULL;
    obhk_cb_record* cb_rec;

    // invoke universal hook
    cb_list = obhook_getcbs_univ( env->eip );
    if( cb_list != NULL ) {
        LL_FOREACH( cb_list, cb_rec ) {
            if( cb_rec->enabled )
                cb_rec->cb_func( env );
        }
    }

    // invoke per-process hook
    cb_list = obhook_getcbs_proc( env->cr[3], env->eip );
    if( cb_list != NULL ) {
        LL_FOREACH( cb_list, cb_rec ) {
            if( cb_rec->enabled )
                cb_rec->cb_func( env );
        }
    }
}
#endif
