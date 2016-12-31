/*
 *  MBA Tracer header
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

#ifndef __TRACER_H__
#define __TRACER_H__

#include <inttypes.h> 

#define MAX_SZ_TRACER_LABEL 256


enum TRACER_ERRNO {
    TRACER_ERR_FAIL,
};


    
//};

enum TRACER_GRANULARITY {
    TRACER_GRANULARITY_INSTR,
    TRACER_GRANULARITY_CODEBLOCK
};


struct tracer_cb_record {

    // unique identifier for each hook
    uint16_t uid;

    bool enabled;
    uint64_t cr3;
    bool is_universal;
    //TODO: Add support for kernel trace 
    //bool is_kernel_trace;
    int trace_granularity;

    // user-friendly label string
    char label[MAX_SZ_TRACER_LABEL];

    void* (*cb_func) (void*);

    struct tracer_cb_record* next;
};

extern int g_enable; 

typedef struct tracer_cb_record tracer_cb_record;

void tracer_enable_tracer(void);
void tracer_disable_tracer(void);
void tracer_list_callback(void);
int tracer_add_process( uint64_t cr3, const char* label, void*(*cb) (void*) );
int tracer_add_universal( const char* label, void*(*cb) (void*) );
extern int tracer_is_enable(void);

#endif
