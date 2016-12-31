/*
 *  MBA Tracer implementation
 *
 *  Copyright (c)   2016 Chiawei Wang
 *                  2016 ChongKuan Chen
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
#include <stdio.h>
#include <stdbool.h>

#include "tracer.h"
#include "qemu-common.h"
#include "utlist.h"

int g_enable = 0;
tracer_cb_record *g_inst_head =NULL;
tracer_cb_record *g_block_head =NULL;

void tracer_enable_tracer(void)
{
    g_enable = 1;
    printf("enable tracer\n");
}

int tracer_is_enable(void)
{
    if(g_enable){
        //printf("Tracer is enabled.\n");
        return 1;
    }
    //printf("Tracer is not enabled.\n");
    return 1;
}

static void* default_callback(void* env)
{
    printf("testtesttest\n");
    return NULL;
}

static int add_tracer_internal( target_ulong cr3, const char* label, void*(*cb) (void*) ) {

    tracer_cb_record* tracer_rec = (tracer_cb_record *) calloc(1, sizeof(tracer_cb_record));
    //Add management for uid
    tracer_rec-> uid = 1;
    strncpy( tracer_rec->label, label, MAX_SZ_TRACER_LABEL );
    tracer_rec->enabled = true;
    tracer_rec->cr3 = cr3;
    tracer_rec->trace_granularity = TRACER_GRANULARITY_INSTR;
    tracer_rec->is_universal = (tracer_rec->cr3 == 0)? true : false;
    tracer_rec->cb_func = cb ;
    
    if(tracer_rec->trace_granularity == TRACER_GRANULARITY_INSTR)
    {
        LL_APPEND(g_inst_head, tracer_rec);
    }
    printf("add new inst tracer\n");
    return 0; 
}


int tracer_add_process( target_ulong cr3, const char* label, void*(*cb) (void*) ) {
    if(cb == NULL) 
        add_tracer_internal(cr3, label, &default_callback);
    else
        add_tracer_internal(cr3, label, cb);
    return 0;
}

int tracer_add_universal( const char* label, void*(*cb) (void*) ) {
    if(cb == NULL)
        add_tracer_internal(0, label, &default_callback);
    else
        add_tracer_internal(0, label, cb);
    return 0;
}

void tracer_list_callback(void){
    tracer_cb_record* rec = NULL;
    LL_FOREACH(g_inst_head, rec)
    {
        printf("cr3: %lx\n", rec->cr3);
        printf("lable: %s\n", rec->label);
        printf("cb: %p\n", rec->cb_func);
    }
}


