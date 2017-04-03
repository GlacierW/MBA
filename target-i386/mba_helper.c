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

#if defined(CONFIG_TRACER)
#include "../ext/tracer/tracer.h"
#include "../ext/tracer/tracer-priv.h"
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
                cb_rec->cb_func( ENV_GET_CPU(env) );
        }
    }

    // invoke per-process hook
    cb_list = obhook_getcbs_proc( env->cr[3], env->eip );
    if( cb_list != NULL ) {
        LL_FOREACH( cb_list, cb_rec ) {
            if( cb_rec->enabled )
                cb_rec->cb_func( ENV_GET_CPU(env) );
        }
    }
}
#endif

#if defined(CONFIG_TRACER)
void helper_tracer_dispatcher( CPUX86State* env, uint64_t pc ) {
    tracer_cb_record *cb_rec = NULL;
    
    if(tracer_ctx.process_tracer_head!=NULL && !tracer_is_kern_addr(pc)){
    	LL_FOREACH( tracer_ctx.process_tracer_head , cb_rec){
        	if( env->cr[3] == cb_rec->cr3 && cb_rec->enabled == 1){
                	cb_rec->cb_func( ENV_GET_CPU(env), pc, 0 );
        	}
    	}
    }

    if( tracer_ctx.universal_kernel_tracer_head!=NULL && tracer_is_kern_addr(pc) ){
            LL_FOREACH( tracer_ctx.universal_kernel_tracer_head , cb_rec){
            if(cb_rec->enabled == 1)
                cb_rec->cb_func( ENV_GET_CPU(env), pc, 0 );
        }
    }

    if( tracer_ctx.universal_tracer_head!=NULL && !tracer_is_kern_addr(pc) ){
            LL_FOREACH( tracer_ctx.universal_tracer_head , cb_rec){
            if(cb_rec->enabled == 1)
                cb_rec->cb_func( ENV_GET_CPU(env), pc, 0 );
        }
    }    
}

void helper_btracer_dispatcher( CPUX86State* env, uint64_t bstart, uint64_t bend ) {
    tracer_cb_record *cb_rec = NULL;
          
    if( tracer_ctx.process_btracer_head!=NULL && !tracer_is_kern_addr(bstart) ){
            LL_FOREACH( tracer_ctx.process_btracer_head , cb_rec){
            if(env->cr[3] == cb_rec->cr3  && cb_rec->enabled == 1)
                cb_rec->cb_func( ENV_GET_CPU(env), bstart, bend );
        }
    }

    if( tracer_ctx.universal_kernel_btracer_head!=NULL && tracer_is_kern_addr(bstart) ){
            LL_FOREACH( tracer_ctx.universal_kernel_btracer_head , cb_rec){
            if(cb_rec->enabled == 1)
                cb_rec->cb_func( ENV_GET_CPU(env), bstart, bend );
        }
    }

    if( tracer_ctx.universal_btracer_head!=NULL && !tracer_is_kern_addr(bstart) ){
            LL_FOREACH( tracer_ctx.universal_btracer_head , cb_rec){
            if(cb_rec->enabled == 1)
                cb_rec->cb_func( ENV_GET_CPU(env), bstart, bend );
        }
    }
}

#endif


